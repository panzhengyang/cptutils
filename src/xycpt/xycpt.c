/*
  xycpt.c

  lots to do here

  (c) J.J.Green 2001,2004
  $Id: gimpcpt.c,v 1.10 2004/03/22 22:58:34 jjg Exp $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "xycpt.h"
#include "cpt.h"
#include "cptio.h"

extern int xycpt(cptopt_t opt)
{
  cpt_t* cpt;
  
  xy = xyread(infile);
    
  if (!xy)
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

    if ((cpt = cpt_new()) == NULL)
      {
	fprintf(stderr,"failed to get new cpt strcture\n");
	return 1;
      }

    cpt->model = rgb;

    cpt->fg.type = cpt->bg.type = cpt->nan.type = colour;

    cpt->fg.u.colour.rgb  = opt.fg;
    cpt->bg.u.colour.rgb  = opt.bg;
    cpt->nan.u.colour.rgb = opt.nan;

    strncpy(cpt->name,(infile ? infile : "<stdin>"),CPT_NAME_LEN);
    
    /* transfer the gradient data to the cpt_t struct */

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
    
    return 0;
}

static int gradcpt(gradient_t* grad,cpt_t* cpt,cptopt_t opt)
{
  grad_segment_t* gseg;
  double bg[3];
  
  if (!grad) return 1;
  
  strncpy(cpt->name,grad->name,CPT_NAME_LEN);
  cpt->model = rgb;
  
  rgb_to_rgbD(opt.trans,bg);
  
  gseg = grad->segments;
  
  while (gseg->next) gseg = gseg->next;
  
  while (gseg)
    {
      cpt_seg_t *lseg,*rseg;
      rgb_t rgb;
      double col[3];
      int err = 0;
      
	if (gseg->type == GRAD_LINEAR && gseg->color == GRAD_RGB)
	  {
	    /* 
	       for linear-rgb segments we can convert naturally without
	       significant distortions, we just look at the 2 subsegments
	       
	       Note that we need to check that the generated segments
	       have positive width (this is permitted in gimp gradients
	       but will cause an error in a cpt files)
	    */
	    
	    /* 
	       create 2 cpt segments -- we dont check whether these are
	       colinear, since we will be performing a cpt_optimise() at 
	       the end anyway.
	    */
	    
	    lseg =  cpt_seg_new();
	    rseg =  cpt_seg_new();
	    
	    if (lseg == NULL || rseg == NULL) return 1;

	    /* the right (we work from right to left) */
	    
	    grad_segment_colour(gseg->right,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    rseg->rsmp.fill.type         = colour; 
	    rseg->rsmp.fill.u.colour.rgb = rgb; 
	    rseg->rsmp.val               = SCALE(gseg->right,opt);

	    grad_segment_colour(gseg->middle,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    rseg->lsmp.fill.type         = colour; 
	    rseg->lsmp.fill.u.colour.rgb = rgb; 
	    rseg->lsmp.val               = SCALE(gseg->middle,opt);

	    if (rseg->lsmp.val < rseg->rsmp.val)
		err |= cpt_prepend(rseg,cpt);

	    /* the left */

	    lseg->rsmp.fill.type         = colour; 
	    lseg->rsmp.fill.u.colour.rgb = rgb; 
	    lseg->rsmp.val               = SCALE(gseg->middle,opt);

	    grad_segment_colour(gseg->left,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    lseg->lsmp.fill.type         = colour; 
	    lseg->lsmp.fill.u.colour.rgb = rgb; 
	    lseg->lsmp.val               = SCALE(gseg->left,opt);

	    if (lseg->lsmp.val < lseg->rsmp.val)
		err |= cpt_prepend(lseg,cpt);

	    if (err)
	      {
		fprintf(stderr,"error converting segment\n");
		return 1;
	      }
	  }
	else
	  {
	    /* 
	       when the segment is non-linear and/or is not RGB, we
	       divide the segment up into small subsegments and write
	       the linear approximations. 
	    */
	    
	    int n,i;
	    double width,x;
	    
	    width = gseg->right - gseg->left;
	    n = (int)(opt.samples*(gseg->right - gseg->left)) + 1;
	    
	    rseg = cpt_seg_new();
	    
	    x = gseg->right;
	    
	    grad_segment_colour(x,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    rseg->rsmp.fill.type         = colour;
	    rseg->rsmp.fill.u.colour.rgb = rgb; 
	    rseg->rsmp.val               = SCALE(x,opt);

	    x = gseg->right-width/n;
	    
	    grad_segment_colour(x,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);
	    
	    rseg->lsmp.fill.type         = colour;
	    rseg->lsmp.fill.u.colour.rgb = rgb; 
	    rseg->lsmp.val               = SCALE(x,opt);
	    
	    cpt_prepend(rseg,cpt);
	    
	    for (i=2 ; i<=n ; i++)
	      {
		lseg = cpt_seg_new();
		
		lseg->rsmp.fill.type         = rseg->lsmp.fill.type;
		lseg->rsmp.fill.u.colour.rgb = rseg->lsmp.fill.u.colour.rgb;
		lseg->rsmp.val               = rseg->lsmp.val;
		
		x = gseg->right-width*i/n;

		grad_segment_colour(x,gseg,bg,col);
		rgbD_to_rgb(col,&rgb);
		
		lseg->lsmp.fill.type         = colour;
		lseg->lsmp.fill.u.colour.rgb = rgb; 
		lseg->lsmp.val               = SCALE(x,opt);
		
		cpt_prepend(lseg,cpt);
		
		rseg = lseg;
	      }
	  }
	
	gseg = gseg->prev;
    } 
  
  return 0;
}

    





