/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimpcpt.c,v 1.3 2004/01/30 00:08:33 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gimpcpt.h"
#include "gradient.h"
#include "findgrad.h"
#include "files.h"
#include "cpt.h"

#define  SCALE(x,opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

static int gradcpt(gradient_t*,cpt_t*,cptopt_t);
static char* find_infile(char*);

extern int gimpcpt(char* infile,char* outfile,cptopt_t opt)
{
    gradient_t* gradient;
    cpt_t*      cpt;

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
	return 1;

    cpt->fg  = opt.fg;
    cpt->bg  = opt.bg;
    cpt->nan = opt.nan;

    cpt->name = (infile ? infile : "<stdin>");
    
    /* transfer the gradient data to the cpt_t struct */

    if (gradcpt(gradient,cpt,opt) != 0)
      {
	fprintf(stderr,"failed to convert gradient\n");
	return 1;
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
  
  cpt->name = strdup(grad->name);
  
  rgb_to_colour(opt.trans,bg);
  
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
	    
	    /* create 2 cpt segments */
	    
	    lseg =  cpt_seg_new();
	    rseg =  cpt_seg_new();
	    
	    if (lseg == NULL || rseg == NULL) return 1;

	    /* the right (we work from right to left) */
	    
	    grad_segment_colour(gseg->right,gseg,bg,col);
	    colour_to_rgb(col,&rgb);

	    rseg->rsmp.rgb = rgb; 
	    rseg->rsmp.val = SCALE(gseg->right,opt);

	    grad_segment_colour(gseg->middle,gseg,bg,col);
	    colour_to_rgb(col,&rgb);

	    rseg->lsmp.rgb = rgb; 
	    rseg->lsmp.val = SCALE(gseg->middle,opt);

	    if (rseg->lsmp.val < rseg->rsmp.val)
		err |= cpt_prepend(rseg,cpt);

	    /* the left */

	    lseg->rsmp.rgb = rgb; 
	    lseg->rsmp.val = SCALE(gseg->middle,opt);

	    grad_segment_colour(gseg->left,gseg,bg,col);
	    colour_to_rgb(col,&rgb);

	    lseg->lsmp.rgb = rgb; 
	    lseg->lsmp.val = SCALE(gseg->left,opt);

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
	       the linear approximations
	    */
	    
	    int n,i;
	    double width,x;
	    
	    width = gseg->right - gseg->left;
	    n = (int)(opt.samples*(gseg->right - gseg->left)) + 1;
	    
	    rseg =  cpt_seg_new();
	    
	    x = gseg->right;
	    
	    grad_segment_colour(x,gseg,bg,col);
	    colour_to_rgb(col,&rgb);
	    
	    rseg->rsmp.rgb = rgb; 
	    rseg->rsmp.val = SCALE(x,opt);

	    x = gseg->right-width/n;
	    
	    grad_segment_colour(x,gseg,bg,col);
	    colour_to_rgb(col,&rgb);
	    
	    rseg->lsmp.rgb = rgb; 
	    rseg->lsmp.val = SCALE(x,opt);
	    
	    cpt_prepend(rseg,cpt);
	    
	    for (i=2 ; i<=n ; i++)
	      {
		lseg =  cpt_seg_new();
		
		lseg->rsmp.rgb = rseg->lsmp.rgb;
		lseg->rsmp.val = rseg->lsmp.val;
		
		x = gseg->right-width*i/n;

		grad_segment_colour(x,gseg,bg,col);
		colour_to_rgb(col,&rgb);
		
		lseg->lsmp.rgb = rgb; 
		lseg->lsmp.val = SCALE(x,opt);
		
		cpt_prepend(lseg,cpt);
		
		rseg = lseg;
	    }
	  }
	
	gseg = gseg->prev;
    } 
  
  return 0;
}

    
    





