/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimpsvg.c,v 1.13 2007/01/25 00:10:40 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gimpsvg.h"

#include "gradient.h"
#include "findgrad.h"
#include "files.h"

#include "svg.h"
#include "svgwrite.h"

#define ERR_CMOD   1
#define ERR_NULL   2
#define ERR_INSERT 3

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
	char* found = NULL;
	
	found = find_infile(infile);
	
	if (found)
	  {
	    if (opt.verbose) printf("gradient file %s\n",found);
	  }
	else
	  {
	    fprintf(stderr,"gradient file %s not found\n",infile);
	    return 1;
	  }
	
	infile = found;
      }
    
    /* load the gradient */
    
    gradient = grad_load_gradient(infile);
    
    if (!gradient)
      {
	fprintf(stderr,"failed to load gradient from ");
	if (infile)
	  {
	    fprintf(stderr,"%s\n",infile);
	    free(infile);
	  }
	else
	  fprintf(stderr,"<stdin>\n");
	
	return 1;
      }
    
    /* create a cpt struct */

    if ((svg = svg_new()) == NULL)
      {
	fprintf(stderr,"failed to get new cpt strcture\n");
	return 1;
      }
    
    /* transfer the gradient data to the cpt_t struct */

    if ((err = gimpsvg_convert(gradient, svg, opt)) != 0)
      {
	switch (err)
	  {
	  case ERR_CMOD:
	    fprintf(stderr,"bad colour model\n");
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
      {
	int n = cpt_nseg(cpt);
	printf("converted to %i segment rgb-spline\n",n);
      }

    /* write the cpt file */
    
    if (cpt_write(outfile,cpt) != 0)
      {
	fprintf(stderr,"failed to write gradient to %s\n",
		(outfile ? outfile : "<stdout>"));
	return 1;
      }
    
    /* tidy */
    
    cpt_destroy(cpt);
    grad_free_gradient(gradient);
    
    return 0;
}

static char* find_infile(const char* infile)
{
  char  *gimp_grads,*found;

  /* try just the name */
  
  found = findgrad_explicit(infile);

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
	  found = findgrad_indir(infile,dir);
	  dir = strtok(NULL,":");
	} 
      free(gimp_grads);
      
      if (found) return found;
    }
  
  /* now try the usual places */
  
  return findgrad_implicit(infile);
}

static int gimpsvg_convert(gradient_t *grad, 
			   svg_t *svg,
			   gimpsvg_opt_t opt)
{
  grad_segment_t* gseg;
  double bg[3];
  
  if (!grad) return 1;
  
  strncpy((char *)svg->name, grad->name, SVG_NAME_LEN);
  
  gseg = grad->segments;
  
  /* final svg stop */

  svg_stop_t stop; 
  rgb_t rgb;
  double rgbD[3], alpha;

  for (n=0,gseg=grad->segment ; gseg ; gseg=gseg->next)
    {
      /* always insert the left colour */

      if (grad_segment_rgba(gseg->left, gseg, rgbD, &alpha) != 0) 
	return ERR_CMOD;
  
      rgbD_to_rgb(rgbD, &stop.rgb);

      stop.value   = 100.0;
      stop.opacity = alpha:

      if (svg_append(stop,svg) != 0) return ERR_INSERT;
      
      /* insert interior segments */

      if (gseg->type == GRAD_LINEAR && gseg->color == GRAD_RGB)
	{
	  if (grad_segment_rgba(gseg->middle, gseg, rgbD, &alpha) != 0) 
	    return ERR_CMOD;
  
	  rgbD_to_rgb(rgbD, &stop.rgb);

	  stop.value   = 100.0 * gseg->middle;
	  stop.opacity = alpha:

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
	  n = (int)(opt.samples*width) + 1;
	    
	  for (i=1 ; i<n ; i++)
	    {
	      double z = gseg->left + i*width/n;

	      if (grad_segment_rgba(z, gseg, rgbD, &alpha) != 0) 
		return ERR_CMOD;
  
	      rgbD_to_rgb(rgbD, &stop.rgb);

	      stop.value   = 100.0;
	      stop.opacity = alpha:

	      if (svg_append(stop,svg) != 0) return ERR_INSERT;
	    }
	}

      /*
	insert right stop if it is not the same as the
	left colour of the next segment
      */ 

      // FIXME

      if (inserr) return ERR_INSERT;
    } 
  
  return 0;
}
    





