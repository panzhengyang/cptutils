/*
  ggr.c

  gradient structures

  This file contains code derived from from gradiant.c
  gradient.h and gradient_header.h, part of the Gimp
  distribution (v1.2). Thanks to the gimp people for making
  this available under the GPL.

  The file give access to the gimp gradient structures
  and some io functionality. I have made a couple of
  changes from the original:

  * structs are now public
  * I removed the glib function calls
  * the pixmap field is removed
  * some cosmetic changes
  * adapted to use the "Name:" line in v1.3 gradients

  It may be useful as a standalone C interface to the gimp
  gradient format.

  gimp/app/gradient.c,gradient.h,gradient_header.h,
  are copyright
    (c) 1995 Spencer Kimball andPeter Mattis,
    with modifications to the gradient editor
    module (c) 1996-1997 Federico Mena Quintero.
  The modifications are
    (c) 2001,2004 J.J.Green (j.j.green@shef.ac.uk)

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
  Free Software Foundation, Inc.,  51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "ggr.h"
#include "btrace.h"

#define EPSILON 1e-10
#define PI 3.141592653

#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)>(b)) ? (b) : (a))

static void            seg_free_segment(grad_segment_t*);
static void            seg_free_segments(grad_segment_t*);
static char*           basename(const char*);
static grad_segment_t* seg_get_segment_at(gradient_t*,double);
static grad_color_t    grad_hsv_type(grad_color_t,double,double);

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

  return grad;
}

extern void grad_free_gradient(gradient_t *grad)
{
  if (grad == NULL) return;

  if (grad->name)     free(grad->name);
  if (grad->filename) free(grad->filename);
  if (grad->segments) seg_free_segments(grad->segments);

  free(grad);
}

static void warn_truncated(const char* where)
{
  btrace("unexpected end of file %s", where);
}

extern gradient_t* grad_load_gradient(const char* path)
{
  FILE           *stream;
  gradient_t     *grad;
  grad_segment_t *seg, *prev;
  int             num_segments;
  int             i;
  char            line[1024];

  if (path == NULL)
    {
       stream = stdin;
    }
  else if ((stream = fopen(path, "rb")) == NULL)
    {
      btrace("failed to open %s : %s", path, strerror(errno));
      return NULL;
    }

  if (fgets(line, 1024, stream) == NULL)
    {
      warn_truncated("at start of file");
      return NULL;
    }

  if (strncmp(line, "GIMP Gradient", 13) != 0)
    {
      btrace("file does not seem to be a GIMP gradient");
      return NULL;
    }

  if ((grad = grad_new_gradient()) == NULL)
    return NULL;

  grad->filename = (path ? strdup(path) : strdup("<stdin>"));

  if (fgets(line, 1024, stream) == NULL)
    {
      warn_truncated("after header");
      return NULL;
    }

  /*
    In 1.3 gradients there is a line with the name of the
    gradient : if we find it then we use that name and read
    another line, otherwise we use the path (or the <stdin>
    string>)
  */

  if (strncmp(line,"Name:",5) == 0)
    {
      char *s,*e;

      for (s = line+5 ; *s && (*s == ' ') ; s++);
      if ((e = strchr(s,'\n')) != NULL) *e = '\0';

      grad->name = strdup(s);

      if (fgets(line, 1024, stream) == NULL)
	{
	  warn_truncated("after segments line");
	  return NULL;
	}
    }
  else
    grad->name = (path ?  basename(path) : strdup("cptutils-output"));

  /* next line specifies number of segments */

  num_segments = atoi(line);

  if (num_segments < 1)
    {
      btrace("invalid number of segments in %s", path);
      free(grad);
      return NULL;
    }

  prev = NULL;

  for (i = 0 ; i < num_segments ; i++)
    {
      int type, color, ect_left, ect_right;

      seg = seg_new_segment();
      seg->prev = prev;

      if (prev)
	prev->next = seg;
      else
	grad->segments = seg;

      if (fgets(line, 1024, stream) == NULL)
	{
	  btrace("unexpected end of file at segment %i", i+1);
	  return NULL;
	}

      switch (sscanf(line, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d%d%d%d",
		     &(seg->left), &(seg->middle), &(seg->right),
		     &(seg->r0), &(seg->g0), &(seg->b0), &(seg->a0),
		     &(seg->r1), &(seg->g1), &(seg->b1), &(seg->a1),
		     &type, &color,
		     &ect_left, &ect_right))
	{
	case 13:

	  ect_left = ect_right = GRAD_FIXED;

	case 15:

	  seg->ect_left  = ect_left;
	  seg->ect_right = ect_right;

          seg->type  = (grad_type_t)type;

	  /* determine colour model (see below) */

	  switch (color)
	    {
	      int err = 0;
	      double rgb0[3], rgb1[3], hsv0[3], hsv1[3];

	    case GRAD_RGB:
	    case GRAD_HSV_CW:
	    case GRAD_HSV_CCW:

	      seg->color = (grad_color_t)color;
	      break;

	    case GRAD_HSV_SHORT:
	    case GRAD_HSV_LONG:

	      rgb0[0] = seg->r0;
	      rgb0[1] = seg->g0;
	      rgb0[2] = seg->b0;

	      err |= rgbD_to_hsvD(rgb0,hsv0);

	      rgb1[0] = seg->r1;
	      rgb1[1] = seg->g1;
	      rgb1[2] = seg->b1;

	      err |= rgbD_to_hsvD(rgb1,hsv1);

	      if (err)
		{
		  btrace("error converting rgb->hsv !");
		  return NULL;
		}

	      seg->color = grad_hsv_type(color,hsv0[0],hsv1[0]);

	      break;

	    default:

	      btrace("unknown colour model (%i)", color);
	      return NULL;
	    }

	  break;

	default:
	  btrace("badly formatted gradient segment %d in %s", i, path);
	  return NULL;
	}
      prev = seg;
    }

  if (stream != stdin) fclose(stream);

  return grad;
}

/*
  this function handles an extension to the gimp gradient colour-arc
  type: the GRAD_HSV_CW and GRAD_HSV_CW are joined by GRAD_HSV_SHORT
  which is an HSV path, either CW or CCW depending on which is
  shorter. Similarly GRAD_HSV_LONG.

  This has been proposed by Neota, who provided the following function
  for its implementation. It should make it into the gimp in 2004.

  If the argument color is GRAD_HSV_SHORT then this function
  returns GRAD_HSV_CW or GRAD_HSV_CCW depending on which hue path
  from x to y is shorter. If the argument is GRAD_HSV_LONG the
  behaviour is reversed.
*/

static grad_color_t grad_hsv_type(grad_color_t color,double x,double y)
{
  double midlen,rndlen,min,max;
  int shorter,result;

  min = MIN(x,y);
  max = MAX(x,y);

  midlen = max - min;
  rndlen = min + (1.0 - max);

  /* find shorter path */

  if (rndlen < midlen)
    shorter = (max==y ? GRAD_HSV_CW : GRAD_HSV_CCW);
  else
    shorter = (max==y ? GRAD_HSV_CCW : GRAD_HSV_CW);

  switch (color)
    {
    case GRAD_HSV_SHORT:
      result = shorter;
      break;
    case GRAD_HSV_LONG:
      result = (shorter == GRAD_HSV_CW ? GRAD_HSV_CCW : GRAD_HSV_CW);
      break;
    default :
      result = color;
    }

  return result;
}

extern int grad_save_gradient(const gradient_t *grad, const char* path)
{
  FILE *stream;
  int num_segments;
  grad_segment_t *seg;

  if (grad == NULL) return 1;

  if (path == NULL)
    {
      stream = stdout;
    }
  else if ((stream = fopen(path, "wb")) == NULL)
    {
      btrace("failed to open %s : %s", path, strerror(errno));
      return 1;
    }

  /*
     File format is:

     GIMP Gradient
     Name: <name>
     number_of_segments
     left middle right r0 g0 b0 a0 r1 g1 b1 a1 type coloring
     left middle right r0 g0 b0 a0 r1 g1 b1 a1 type coloring
     ...
  */

  fprintf(stream,"GIMP Gradient\n");
  fprintf(stream,"Name: %s\n",grad->name);

  /* Count number of segments */

  num_segments = 0;
  seg = grad->segments;
  while (seg)
    {
      num_segments++;
      seg = seg->next;
    }

  /* Write rest of file */

  fprintf(stream, "%d\n", num_segments);

  for (seg = grad->segments; seg; seg = seg->next)
    {
      if ( ( seg->ect_left == GRAD_FIXED) &&
	   ( seg->ect_right == GRAD_FIXED) )
	{
	  fprintf(stream, "%f %f %f %f %f %f %f %f %f %f %f %d %d\n",
		  seg->left, seg->middle, seg->right,
		  seg->r0, seg->g0, seg->b0, seg->a0,
		  seg->r1, seg->g1, seg->b1, seg->a1,
		  (int) seg->type, (int) seg->color);
	}
      else
	{
	  fprintf(stream, "%f %f %f %f %f %f %f %f %f %f %f %d %d %d %d\n",
		  seg->left, seg->middle, seg->right,
		  seg->r0, seg->g0, seg->b0, seg->a0,
		  seg->r1, seg->g1, seg->b1, seg->a1,
		  (int) seg->type, (int) seg->color,
		  (int) seg->ect_left, (int) seg->ect_right);
	}
    }


  if (stream != stdout) fclose(stream);

  return 0;
}

extern int grad_segment_colour(double z, const grad_segment_t* seg,
			       double *bgD, double *rgbD)
{
  double alpha;
  int err;

  if ((err = grad_segment_rgba(z, seg, rgbD, &alpha)) != 0)
    return err;

  int i;

  for (i=0 ; i<3 ; i++)
    rgbD[i] = alpha*rgbD[i] + (1-alpha)*bgD[i];

  return 0;
}

extern int grad_segment_rgba(double z,
			     const grad_segment_t *seg,
			     double *rgbD,
			     double *alpha)
{
  double factor = 0;
  double seg_len, middle;

  seg_len = seg->right - seg->left;

  if (seg_len < EPSILON)
    {
      middle = 0.5;
      z = 0.5;
    }
  else
    {
      middle = (seg->middle - seg->left) / seg_len;
      z = (z - seg->left) / seg_len;
    }

  switch (seg->type)
    {
    case GRAD_LINEAR:
      factor = calc_linear_factor(middle, z);
      break;
    case GRAD_CURVED:
      factor = calc_curved_factor(middle, z);
      break;
    case GRAD_SINE:
      factor = calc_sine_factor(middle, z);
      break;
    case GRAD_SPHERE_INCREASING:
      factor = calc_sphere_increasing_factor(middle, z);
      break;
    case GRAD_SPHERE_DECREASING:
      factor = calc_sphere_decreasing_factor(middle, z);
      break;
    default:
      btrace("Corrupt gradient");
      return 1;
    }

  /* alpha channel is easy */

  *alpha = seg->a0 + (seg->a1 - seg->a0)*factor;

  /* Calculate color components */

  if (seg->color == GRAD_RGB)
    {
      rgbD[0] = seg->r0 + (seg->r1 - seg->r0)*factor;
      rgbD[1] = seg->g0 + (seg->g1 - seg->g0)*factor;
      rgbD[2] = seg->b0 + (seg->b1 - seg->b0)*factor;
    }
  else
    {
      double  h0,s0,v0,h1,s1,v1;
      double hsvD[3];

      rgbD[0] = seg->r0;
      rgbD[1] = seg->g0;
      rgbD[2] = seg->b0;

      rgbD_to_hsvD(rgbD,hsvD);

      h0 = hsvD[0];
      s0 = hsvD[1];
      v0 = hsvD[2];

      rgbD[0] = seg->r1;
      rgbD[1] = seg->g1;
      rgbD[2] = seg->b1;

      rgbD_to_hsvD(rgbD,hsvD);

      h1 = hsvD[0];
      s1 = hsvD[1];
      v1 = hsvD[2];

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
	  btrace("unknown colour model %i",seg->color);
	  return 1;
	}

      hsvD[0] = h0;
      hsvD[1] = s0;
      hsvD[2] = v0;

      hsvD_to_rgbD(hsvD,rgbD);
   }

  return 0;
}


extern int gradient_colour(double z, gradient_t *gradient,
			   double *bg, double *rgbD)
{
  /* if there is no gradient return the background colour */

  if (gradient == NULL)
    {
      int i;

      for (i=0 ; i<3 ; i++) rgbD[i] = bg[i];
      return 0;
    }

  if (z < 0.0)
    z = 0.0;
  else if (z > 1.0)
    z = 1.0;

  grad_segment_t *seg;

  if ((seg = seg_get_segment_at(gradient, z)) == NULL)
    return 1;

  return grad_segment_colour(z, seg, bg, rgbD);
}

extern grad_segment_t* seg_new_segment(void)
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

static grad_segment_t* seg_get_segment_at(gradient_t *grad, double z)
{
  grad_segment_t *seg;

  z = MIN(z, 1.0);
  z = MAX(z, 0.0);

  if (grad->last_visited)
    seg = grad->last_visited;
  else
    seg = grad->segments;

  while (seg)
    {
      if (z >= seg->left)
	{
	  if (z <= seg->right)
	    {
	      grad->last_visited = seg; /* for speed */
	      return seg;
	    }
	  else
	    seg = seg->next;
	}
      else
	seg = seg->prev;
    }

  btrace("no matching segment for z %0.15f", z);

  return NULL;
}

/*
  calculation functions
*/

static double calc_linear_factor(double middle, double z)
{
  if (z <= middle)
    {
      if (middle < EPSILON)
	return 0.0;
      else
	return 0.5 * z / middle;
    }
  else
    {
      z -= middle;
      middle = 1.0 - middle;

      if (middle < EPSILON)
	return 1.0;
      else
	return 0.5 + 0.5 * z / middle;
    }
}

static double calc_curved_factor(double middle,double z)
{
  if (middle < EPSILON)
    middle = EPSILON;

  return pow(z,log(0.5)/log(middle));
}

static double calc_sine_factor(double middle,double z)
{
  z = calc_linear_factor(middle, z);

  return (sin((-PI/2.0) + PI*z) + 1.0)/2.0;
}

/* Works for convex increasing and concave decreasing */

static double calc_sphere_increasing_factor(double middle, double z)
{
  z = calc_linear_factor(middle, z) - 1.0;

  return sqrt(1.0 - z*z);
}

/* Works for convex decreasing and concave increasing */

static double calc_sphere_decreasing_factor(double middle, double z)
{
  z = calc_linear_factor(middle, z);

  return 1.0 - sqrt(1.0 - z*z);
}

/*
  find the last point in a string and duplicate the
  string up to that position
*/

static char* basename(const char* name)
{
   char *base,*last;

   if (name == NULL) return NULL;

   if ((base = strdup(name)) == NULL) return NULL;

   if ((last = strrchr(base,'.')) != NULL)
     *last = '\0';

   return base;
}
