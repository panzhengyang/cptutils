/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimpcpt.c,v 1.12 2007/01/24 21:03:24 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gimpcpt.h"
#include "gradient.h"
#include "findgrad.h"
#include "files.h"
#include "cptio.h"

/*
  #define DEBUG
*/

#define SCALE(x,opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

#define ERR_CMOD   1
#define ERR_NULL   2
#define ERR_INSERT 3

static int gradcpt(gradient_t*,cpt_t*,cptopt_t);

static char* find_infile(char*);

extern int gimpcpt(char* infile,char* outfile,cptopt_t opt)
{
    gradient_t* gradient;
    cpt_t*      cpt;
    int         err;

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

    if ((err = gradcpt(gradient,cpt,opt)) != 0)
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

static char* find_infile(char* infile)
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
      int inserr = 0;
      
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
	    
	    if (lseg == NULL || rseg == NULL) return ERR_NULL;

	    /* the right (we work from right to left) */
	    
	    if (grad_segment_colour(gseg->right,gseg,bg,col) != 0) return ERR_CMOD;
	    rgbD_to_rgb(col,&rgb);

	    rseg->rsmp.fill.type         = colour; 
	    rseg->rsmp.fill.u.colour.rgb = rgb; 
	    rseg->rsmp.val               = SCALE(gseg->right,opt);

	    if (grad_segment_colour(gseg->middle,gseg,bg,col) != 0) return ERR_CMOD;
	    rgbD_to_rgb(col,&rgb);

	    rseg->lsmp.fill.type         = colour; 
	    rseg->lsmp.fill.u.colour.rgb = rgb; 
	    rseg->lsmp.val               = SCALE(gseg->middle,opt);

	    if (rseg->lsmp.val < rseg->rsmp.val)
		inserr |= cpt_prepend(rseg,cpt);

	    /* the left */

	    lseg->rsmp.fill.type         = colour; 
	    lseg->rsmp.fill.u.colour.rgb = rgb; 
	    lseg->rsmp.val               = SCALE(gseg->middle,opt);

	    if (grad_segment_colour(gseg->left,gseg,bg,col) != 0) return ERR_CMOD;
	    rgbD_to_rgb(col,&rgb);

	    lseg->lsmp.fill.type         = colour; 
	    lseg->lsmp.fill.u.colour.rgb = rgb; 
	    lseg->lsmp.val               = SCALE(gseg->left,opt);

	    if (lseg->lsmp.val < lseg->rsmp.val)
		inserr |= cpt_prepend(lseg,cpt);
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
	    
	    if ((rseg = cpt_seg_new()) == NULL) return ERR_NULL;
	    
	    x = gseg->right;
	    
	    if (grad_segment_colour(x,gseg,bg,col) != 0) return ERR_CMOD;
	    rgbD_to_rgb(col,&rgb);

	    rseg->rsmp.fill.type         = colour;
	    rseg->rsmp.fill.u.colour.rgb = rgb; 
	    rseg->rsmp.val               = SCALE(x,opt);

	    x = gseg->right-width/n;
	    
	    if (grad_segment_colour(x,gseg,bg,col) != 0) return ERR_CMOD;
	    rgbD_to_rgb(col,&rgb);
	    
	    rseg->lsmp.fill.type         = colour;
	    rseg->lsmp.fill.u.colour.rgb = rgb; 
	    rseg->lsmp.val               = SCALE(x,opt);
	    
	    inserr |= cpt_prepend(rseg,cpt);
	    
	    for (i=2 ; i<=n ; i++)
	      {
		if ((lseg = cpt_seg_new()) == NULL) return ERR_NULL;
		
		lseg->rsmp.fill.type         = rseg->lsmp.fill.type;
		lseg->rsmp.fill.u.colour.rgb = rseg->lsmp.fill.u.colour.rgb;
		lseg->rsmp.val               = rseg->lsmp.val;
		
		x = gseg->right-width*i/n;

		if (grad_segment_colour(x,gseg,bg,col) != 0) return ERR_CMOD;
		rgbD_to_rgb(col,&rgb);
		
		lseg->lsmp.fill.type         = colour;
		lseg->lsmp.fill.u.colour.rgb = rgb; 
		lseg->lsmp.val               = SCALE(x,opt);
		
		inserr |= cpt_prepend(lseg,cpt);
		
		rseg = lseg;
	      }
	  }
	
	if (inserr) return ERR_INSERT;

	gseg = gseg->prev;
    } 
  
  return 0;
}
    





