/*
  gradient.c

  gradient structures

  This file contains code derived from from gradiant.c
  gradient.h and gradient_header.h, part of the Gimp
  distribution. Thanks to the Gimp people for making
  this available under the GPL.

  The file give access to the Gimp gradient structures
  and some io functionality. I have made a couple of
  changes from the original:
  * structs are now public
  * I removed the glib function calls
  * the pixmap field is missing (I'll look at this later)

  gimp/app/gradient.c,gradient.h,gradient_header.h,
  are copyright
    (c) 1995 Spencer Kimball andPeter Mattis,
    with modifications to the gradient editor
    module (c) 1996-1997 Federico Mena Quintero.
  The rest of this program is
    (c) 2001 J.J.Green (j.j.green@shef.ac.uk)

  This program is free software; you can redistribute it
  and/or modify it under the terms of the GNU General
  Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A
  PARTICULAR PURIGHTE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public
  License along with this program; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.

  $Id: gradient.c,v 1.1 2002/06/18 22:25:32 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gradient.h"

#define EPSILON 1e-10
#define PI 3.141592653


#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)>(b)) ? (b) : (a))

static grad_segment_t* seg_new_segment(void);
static void            seg_free_segment(grad_segment_t*);
static void            seg_free_segments(grad_segment_t*);
static char*           basename(char*);
static grad_segment_t* seg_get_segment_at(gradient_t*,double);

static double calc_linear_factor            (double middle, double pos);
static double calc_curved_factor            (double middle, double pos);
static double calc_sine_factor              (double middle, double pos);
static double calc_sphere_increasing_factor (double middle, double pos);
static double calc_sphere_decreasing_factor (double middle, double pos);

extern gradient_t *grad_new_gradient(void)
{
  gradient_t *grad;

  if ((grad = malloc(sizeof(gradient_t))) == NULL)
    return NULL;

  grad->name   	     = NULL;
  grad->filename     = NULL;
  grad->segments     = NULL;
  grad->last_visited = NULL;

  /*
    grad->pixmap  = NULL;
  */
  	
  return grad;
}

extern void grad_free_gradient(gradient_t *grad)
{
  if (grad == NULL) return;

  if (grad->name) free(grad->name);
  if (grad->filename) free(grad->filename);
  if (grad->segments) seg_free_segments(grad->segments);

  /*
  if (grad->pixmap) gdk_pixmap_unref(grad->pixmap);
  */

  free(grad);
}

extern gradient_t* grad_load_gradient(char* filename)
{
  FILE           *stream;
  gradient_t     *grad;
  grad_segment_t *seg, *prev;
  int             num_segments;
  int             i;
  int             type, color;
  char            line[1024];

  if (filename == NULL)
    {
       stream = stdin;
    }
  else if ((stream = fopen(filename,"rb")) == NULL)
    {
      fprintf(stderr,"file open (r) failed for %s\n",filename);
      return NULL;
    }

  fgets(line, 1024, stream);
  if (strcmp(line, "GIMP Gradient\n") != 0)
     return NULL;

  if ((grad = grad_new_gradient()) == NULL)
     return NULL;

  grad->filename = (filename ? strdup(filename) : strdup("<stdin>"));

  fgets(line, 1024, stream);

  /*
    In 1.3 gradients there is a line with the name of the 
    gradient : if we find it then we use that name and read
    another line, otherwise we use the filename (or the <stdin>
    string>)
  */

  if (strncmp(line,"Name:",5) == 0)
    {
      char *s,*e;

      for (s = line+5 ; *s && (*s == ' ') ; s++);
      if ((e = strchr(s,'\n')) != NULL) *e = '\0';
      
      grad->name = strdup(s);

      fgets(line, 1024, stream);
    }
  else
    grad->name = (filename ?  basename(filename) : strdup("gimpcpt-output"));

  /* next line specifies number of segments */

  num_segments = atoi(line);

  if (num_segments < 1)
    {
	fprintf(stderr,
		"invalid number of segments in %s\n",
		filename);
      free(grad);
      return NULL;
    }

  prev = NULL;

  for (i = 0 ; i < num_segments ; i++)
    {
      seg = seg_new_segment();
      seg->prev = prev;

      if (prev)
	prev->next = seg;
      else
	grad->segments = seg;

      fgets (line, 1024, stream);
      if (sscanf (line, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d%d",
		  &(seg->left), &(seg->middle), &(seg->right),
		  &(seg->r0), &(seg->g0), &(seg->b0), &(seg->a0),
		  &(seg->r1), &(seg->g1), &(seg->b1), &(seg->a1),
		  &type, &color) != 13)
	{
	  fprintf(stderr,
		  "badly formatted gradient segment %d in %s\n",
		  i, filename);
	}
      else
	{
          seg->type  = (grad_type_t)type;
	  seg->color = (grad_color_t)color;
	}      
      prev = seg;
    }

  if (stream != stdin) fclose(stream);

  return grad;
}

extern int grad_save_gradient(gradient_t *grad,char* filename)
{
  FILE           *stream;
  int             num_segments;
  grad_segment_t *seg;

  if (grad == NULL) return 1;

  if (filename == NULL)
    {
      stream = stdout;
    }
  else if ((stream = fopen(filename, "wb")) == NULL)
    {
      fprintf(stderr,
	      "file open (w) failed for %s\n",
	      filename);
      return 1;
    }

  /* File format is:
   *
   *   GIMP Gradient
   *   number_of_segments
   *   left middle right r0 g0 b0 a0 r1 g1 b1 a1 type coloring
   *   left middle right r0 g0 b0 a0 r1 g1 b1 a1 type coloring
   *   ...
   */

  fprintf (stream,"GIMP Gradient\n");

  /* Count number of segments */

  num_segments = 0;
  seg = grad->segments;
  while (seg)
    {
      num_segments++;
      seg = seg->next;
    }

  /* Write rest of file */

  fprintf (stream, "%d\n", num_segments);

  for (seg = grad->segments; seg; seg = seg->next)
    fprintf (stream, "%f %f %f %f %f %f %f %f %f %f %f %d %d\n",
	     seg->left, seg->middle, seg->right,
	     seg->r0, seg->g0, seg->b0, seg->a0,
	     seg->r1, seg->g1, seg->b1, seg->a1,
	     (int) seg->type, (int) seg->color);

  if (stream != stdout) fclose(stream);

  return 0;
}

/* not used
static gradient_t * grad_create_default_gradient(void)
{
  gradient_t *grad;

  grad = grad_new_gradient();
  grad->segments = seg_new_segment();

  return grad;
}
*/

extern int grad_segment_colour(double pos,grad_segment_t* seg,double* bg,double* col)
{
    double          factor=0.0;
    double          seg_len,middle,a;
    int i;

    seg_len = seg->right - seg->left;

    if (seg_len < EPSILON)
    {
	middle = 0.5;
	pos    = 0.5;
    }
    else
    {
	middle = (seg->middle - seg->left) / seg_len;
	pos    = (pos - seg->left) / seg_len;
    }
    switch (seg->type)
    {
	case GRAD_LINEAR:
	    factor = calc_linear_factor(middle, pos);
	    break;
	case GRAD_CURVED:
	    factor = calc_curved_factor(middle, pos);
	    break;
	case GRAD_SINE:
	    factor = calc_sine_factor(middle, pos);
	    break;
	case GRAD_SPHERE_INCREASING:
	    factor = calc_sphere_increasing_factor(middle, pos);
	    break;
	case GRAD_SPHERE_DECREASING:
	    factor = calc_sphere_decreasing_factor(middle, pos);
	    break;
	default:
	    fprintf(stderr,"Corrupt gradient\n");
	    return 1;
    }


    /* Calculate color components */

    a = seg->a0 + (seg->a1 - seg->a0)*factor;

    if (seg->color == GRAD_RGB)
    {
	col[0] = seg->r0 + (seg->r1 - seg->r0)*factor;
	col[1] = seg->g0 + (seg->g1 - seg->g0)*factor;
	col[2] = seg->b0 + (seg->b1 - seg->b0)*factor;
    }
    else
    {
	double  h0,s0,v0,h1,s1,v1;

	rgb_to_hsv(seg->r0,seg->g0,seg->b0,&h0,&s0,&v0);
	rgb_to_hsv(seg->r1,seg->g1,seg->b1,&h1,&s1,&v1);

	s0 = s0 + (s1 - s0)*factor;
	v0 = v0 + (v1 - v0)*factor;
      
	switch (seg->color)
	{
	    case GRAD_HSV_CCW:
		if (h0 < h1)
		    h0 = h0 + (h1 - h0)*factor;
		else
		{
		    h0 = h0 + (1.0 - (h0 - h1))*factor;
		    if (h0 > 1.0)
			h0 -= 1.0;
		}
		break;
	    case GRAD_HSV_CW:
		if (h1 < h0)
		    h0 = h0 - (h0 - h1)*factor;
		else
		{
		    h0 = h0 - (1.0 - (h1 - h0))*factor;
		    if (h0 < 0.0)
			h0 += 1.0;
		}
		break;
	    default:
		fprintf(stderr,"unknown colur model\n");
		return 1;
	}
	  
	hsv_to_rgb(h0,s0,v0,col,col+1,col+2);
    }

    for (i=0 ; i<3 ; i++) col[i] = a*col[i] + (1-a)*bg[i];

    return 0;
}


extern int gradient_colour(double pos,gradient_t *gradient,
			   double* bg,double* col)
{
    grad_segment_t *seg;


    /* if there is no gradient return the background colour */

    if (gradient == NULL) 
    {
	int i;

	for (i=0 ; i<3 ; i++) col[i] = bg[i];
	return 0;
    }


    if (pos < 0.0) pos = 0.0;
    else if (pos > 1.0) pos = 1.0;

    seg = seg_get_segment_at(gradient, pos);

    return grad_segment_colour(pos,seg,bg,col);
}


static grad_segment_t* seg_new_segment(void)
{
  grad_segment_t *seg;

  if ((seg = malloc(sizeof(grad_segment_t))) == NULL)
    return NULL;         	

  seg->left   = 0.0;
  seg->middle = 0.5;
  seg->right  = 1.0;

  seg->r0 = seg->g0 = seg->b0 = 0.0;
  seg->r1 = seg->g1 = seg->b1 = seg->a0 = seg->a1 = 1.0;

  seg->type  = GRAD_LINEAR;
  seg->color = GRAD_RGB;

  seg->prev = seg->next = NULL;

  return seg;
}

static void seg_free_segment(grad_segment_t *seg)
{
  if (seg) free(seg);
}

static void seg_free_segments(grad_segment_t *seg)
{
  grad_segment_t *tmp;

  if (seg == NULL) return;

  while (seg)
    {
      tmp = seg->next;
      seg_free_segment (seg);
      seg = tmp;
    }
}

static grad_segment_t* seg_get_segment_at(gradient_t *grad,double pos)
{
  grad_segment_t *seg;

  pos = MIN(pos,1.0);
  pos = MAX(pos,0.0);

  if (grad->last_visited)
      seg = grad->last_visited;
  else
      seg = grad->segments;

  while (seg)
  {
      if (pos >= seg->left)
      {
	  if (pos <= seg->right)
	  {
	      grad->last_visited = seg; /* for speed */
	      return seg;
	  }
	  else
	  {
	      seg = seg->next;
	  }
      }
      else
      {
	  seg = seg->prev;
      }
  }
  
  fprintf(stderr,"No matching segment for position %0.15f", pos);
  
  return NULL;
}

/*
  calculation functions 
*/

static double calc_linear_factor (double middle,double pos)
{
  if (pos <= middle)
    {
      if (middle < EPSILON)
	return 0.0;
      else
	return 0.5 * pos / middle;
    }
  else
    {
      pos -= middle;
      middle = 1.0 - middle;

      if (middle < EPSILON)
	return 1.0;
      else
	return 0.5 + 0.5 * pos / middle;
    }
}


static double calc_curved_factor(double middle,double pos)
{
  if (middle < EPSILON)
    middle = EPSILON;

  return pow(pos,log(0.5)/log(middle));
}

static double calc_sine_factor(double middle,double pos)
{
  pos = calc_linear_factor (middle, pos);

  return (sin((-PI/2.0) + PI*pos) + 1.0)/2.0;
}

static double calc_sphere_increasing_factor(double middle,double pos)
{
  pos = calc_linear_factor(middle,pos) - 1.0;

  /* Works for convex increasing and concave decreasing */
  return sqrt (1.0 - pos*pos); 
}
static double calc_sphere_decreasing_factor(double middle,double pos)
{
  pos = calc_linear_factor (middle, pos);

 /* Works for convex decreasing and concave increasing */
  return 1.0 - sqrt(1.0 - pos*pos);
}

/*
  find the last point in a string and duplicate the
  string up to that position
*/

static char* basename(char* name)
{
   char *base,*last;

   if (name == NULL) return NULL;

   if ((base = strdup(name)) == NULL) return NULL;

   if ((last = strrchr(base,'.')) != NULL)
     *last = '\0';

   return base;
}































