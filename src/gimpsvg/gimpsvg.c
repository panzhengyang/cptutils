/*
  gimpcpt.c

  (c) J.J.Green 2011
  $Id: gimpsvg.c,v 1.17 2012/04/17 17:42:20 jjg Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gimpsvg.h"

#include "ggr.h"
#include "findggr.h"
#include "files.h"

#include "svg.h"
#include "svgwrite.h"

#define ERR_SEGMENT_RGBA 1
#define ERR_NULL         2
#define ERR_INSERT       3

static int gimpsvg_convert(gradient_t*, svg_t*, gimpsvg_opt_t);

static char* find_infile(const char*);

extern int gimpsvg(const char *infile, 
		   const char *outfile, 
		   gimpsvg_opt_t opt)
{
    gradient_t *gradient;
    svg_t *svg;
    int err;

    /* get the full filename */
    
    if (infile)
      {
	char* found;
		
	if ((found = find_infile(infile)) == NULL)
	  {
	    fprintf(stderr,"gradient file %s not found\n",infile);
	    return 1;
	  }
	else
	  {
	    if (opt.verbose) printf("gradient file %s\n",found);
	  }
	
	infile = found;
      }
    
    /* load the gradient */
    
    if ((gradient = grad_load_gradient(infile)) == NULL)
      {
	fprintf(stderr,"failed to load gradient from %s",
		(infile ? infile : "<stdin>"));
	return 1;
      }
    
    /* create a svg struct */

    if ((svg = svg_new()) == NULL)
      {
	fprintf(stderr,"failed to get new cpt strcture\n");
	return 1;
      }
    
    /* transfer the gradient data to the svg_t struct */

    if ((err = gimpsvg_convert(gradient, svg, opt)) != 0)
      {
	switch (err)
	  {
	  case ERR_SEGMENT_RGBA:
	    fprintf(stderr,"error gretting colour from segment\n");
	    break;
	  case ERR_NULL:
	    fprintf(stderr,"null structure\n");
	    break;
	  case ERR_INSERT:
	    fprintf(stderr,"failed structure insert\n");
	    break;
	  default:
	    fprintf(stderr,"unknown error\n");
	  }
	return 1;
      }

    if (opt.verbose) 
      printf("converted to %i stops\n",svg_num_stops(svg));

    /* write the cpt file */
    
    if (svg_write(outfile, svg, &(opt.preview)) != 0)
      {
	fprintf(stderr,"failed to write gradient to %s\n",
		(outfile ? outfile : "<stdout>"));
	return 1;
      }
    
    /* tidy */
    
    svg_destroy(svg);
    grad_free_gradient(gradient);
    
    return 0;
}

static char* find_infile(const char* infile)
{
  char  *gimp_grads, *found;

  /* try just the name */
  
  found = findggr_explicit(infile);

  if (found) return found;
  else if (absolute_filename(infile)) return NULL;
  
  /* check the GIMP_GRADIENTS directories */
  
  if ((gimp_grads = getenv("GIMP_GRADIENTS")) != NULL)
    {
      char* dir;
      
      if ((gimp_grads = strdup(gimp_grads)) == NULL)
	return NULL;
      
      dir = strtok(gimp_grads,":");

      while (dir && !found)
	{
	  found = findggr_indir(infile,dir);
	  dir = strtok(NULL,":");
	} 

      free(gimp_grads);
      
      if (found) return found;
    }
  
  /* now try the usual places */
  
  return findggr_implicit(infile);
}

#define EPSRGB   (0.5 / 256.0)
#define EPSALPHA 1e-4

static int grad_segment_jump(grad_segment_t *lseg, grad_segment_t *rseg)
{
  return ( (fabs(lseg->r1 - rseg->r0) > EPSRGB) ||
	   (fabs(lseg->g1 - rseg->g0) > EPSRGB) ||
	   (fabs(lseg->b1 - rseg->b0) > EPSRGB) ||
	   (fabs(lseg->a1 - rseg->a0) > EPSALPHA) );
}

static int gimpsvg_convert(gradient_t *grad, 
			   svg_t *svg,
			   gimpsvg_opt_t opt)
{
  grad_segment_t *gseg;
  
  if (!grad) return 1;
  
  strncpy((char *)svg->name, grad->name, SVG_NAME_LEN);
  
  gseg = grad->segments;
  
  /* final svg stop */

  svg_stop_t stop; 
  double rgbD[3], alpha;

  for (gseg=grad->segments ; gseg ; gseg=gseg->next)
    {
      /* always insert the left colour */

      if (grad_segment_rgba(gseg->left, gseg, rgbD, &alpha) != 0) 
	return ERR_SEGMENT_RGBA;
  
      rgbD_to_rgb(rgbD, &stop.colour);

      stop.value   = 100.0 * gseg->left;
      stop.opacity = alpha;

      if (svg_append(stop,svg) != 0) return ERR_INSERT;
      
      /* insert interior segments */

      if (gseg->type == GRAD_LINEAR && gseg->color == GRAD_RGB)
	{
	  if (grad_segment_rgba(gseg->middle, gseg, rgbD, &alpha) != 0)
	    return ERR_SEGMENT_RGBA; 
  
	  rgbD_to_rgb(rgbD, &stop.colour);

	  stop.value   = 100.0 * gseg->middle;
	  stop.opacity = alpha;

	  if (svg_append(stop,svg) != 0) return ERR_INSERT;
	}
      else
	{
	  /*
	    when the segment is non-linear and/or is not RGB, we
	    divide the segment up into small subsegments and write
	    the linear approximations. 
	  */
	    
	  int m,i;
	  double width;
	    
	  width = gseg->right - gseg->left;
	  m = (int)(opt.samples*width) + 1;
	    
	  for (i=1 ; i<m ; i++)
	    {
	      double z = gseg->left + i*width/m;

	      if (grad_segment_rgba(z, gseg, rgbD, &alpha) != 0) 
		return ERR_SEGMENT_RGBA;

	      rgbD_to_rgb(rgbD, &stop.colour);

	      stop.value   = 100.0 * z;
	      stop.opacity = alpha;

	      if (svg_append(stop,svg) != 0) return ERR_INSERT;
	    }
	}

      /*
	insert right stop if it is not the same as the
	left colour of the next segment
      */ 

      if ( (! gseg->next) || grad_segment_jump(gseg,gseg->next) )
	{
	  if (grad_segment_rgba(gseg->right, gseg, rgbD, &alpha) != 0)
	    return ERR_SEGMENT_RGBA; 
  
	  rgbD_to_rgb(rgbD, &stop.colour);

	  stop.value   = 100.0 * gseg->right;
	  stop.opacity = alpha;

	  if (svg_append(stop,svg) != 0) return ERR_INSERT;
	}
    } 
  
  return 0;
}


