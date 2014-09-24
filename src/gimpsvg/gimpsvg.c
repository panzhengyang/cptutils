/*
  gimpcpt.c

  (c) J.J.Green 2011, 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ggr.h>
#include <svg.h>
#include <svgwrite.h>
#include <btrace.h>

#include "gimpsvg.h"

#define ERR_SEGMENT_RGBA 1
#define ERR_NULL         2
#define ERR_INSERT       3

static int gimpsvg_convert(gradient_t*, svg_t*, gimpsvg_opt_t);

extern int gimpsvg(const char *infile, 
		   const char *outfile, 
		   gimpsvg_opt_t opt)
{
    gradient_t *gradient;
    svg_t *svg;
    int err;

    /* load the gradient */
    
    if ((gradient = grad_load_gradient(infile)) == NULL)
      {
	btrace_add("failed to load gradient from %s",
		   (infile ? infile : "<stdin>"));
	return 1;
      }
    
    /* create a svg struct */

    if ((svg = svg_new()) == NULL)
      {
	btrace_add("failed to get new cpt strcture");
	return 1;
      }
    
    /* transfer the gradient data to the svg_t struct */

    if ((err = gimpsvg_convert(gradient, svg, opt)) != 0)
      {
	switch (err)
	  {
	  case ERR_SEGMENT_RGBA:
	    btrace_add("error gretting colour from segment");
	    break;
	  case ERR_NULL:
	    btrace_add("null structure");
	    break;
	  case ERR_INSERT:
	    btrace_add("failed structure insert");
	    break;
	  default:
	    btrace_add("unknown error");
	  }
	return 1;
      }

    if (opt.verbose) 
      printf("converted to %i stops\n",svg_num_stops(svg));

    /* write the cpt file */
    
    if (svg_write(outfile, 1, (const svg_t**)(&svg), &(opt.preview)) != 0)
      {
	btrace_add("failed to write gradient to %s",
		   (outfile ? outfile : "<stdout>"));
	return 1;
      }
    
    /* tidy */
    
    svg_destroy(svg);
    grad_free_gradient(gradient);
    
    return 0;
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
  if (!grad) return 1;
  
  strncpy((char *)svg->name, grad->name, SVG_NAME_LEN);
  
  svg_stop_t stop; 
  double rgbD[3], alpha;
  grad_segment_t *gseg;

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


