/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimpcpt.c,v 1.8 2004/02/24 02:13:45 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gimpcpt.h"
#include "gradient.h"
#include "findgrad.h"
#include "files.h"
#include "cpt.h"
#include "dp-simplify.h"

/*
#define DEBUG
*/

#define SCALE(x,opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

static int gradcpt(gradient_t*,cpt_t*,cptopt_t);
static int cpt_optimise(double,cpt_t*);

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
      {
	fprintf(stderr,"failed to get new cpt strcture\n");
	return 1;
      }

    cpt->model = rgb;

    cpt->fg.rgb  = opt.fg;
    cpt->bg.rgb  = opt.bg;
    cpt->nan.rgb = opt.nan;

    strncpy(cpt->name,(infile ? infile : "<stdin>"),CPT_NAME_LEN);
    
    /* transfer the gradient data to the cpt_t struct */

    if (gradcpt(gradient,cpt,opt) != 0)
      {
	fprintf(stderr,"failed to convert gradient\n");
	return 1;
      }

    if (opt.verbose) 
      {
	int n = cpt_nseg(cpt);
	printf("converted to %i segment rgb-spline\n",n);
      }

    /* perform optimisation */

    if (opt.tol > 0.0)
      {
	if (opt.verbose) 
	  printf("optimising spline with tolerance %.2f pixels\n",opt.tol);
    	
	if (cpt_optimise(opt.tol/255.0,cpt) != 0)
	  {
	    fprintf(stderr,"failed to optimise cpt\n");
	    return 1;
	  }

	if (opt.verbose) 
	  {
	    int n = cpt_nseg(cpt);
	    printf("optimised to %i segment%s\n",n,(n-1 ? "s" : ""));
	  }
      }    
    else
      {
	if (opt.verbose)
	  printf("skipping optimisation\n");
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

	    rseg->rsmp.col.rgb = rgb; 
	    rseg->rsmp.val     = SCALE(gseg->right,opt);

	    grad_segment_colour(gseg->middle,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    rseg->lsmp.col.rgb = rgb; 
	    rseg->lsmp.val     = SCALE(gseg->middle,opt);

	    if (rseg->lsmp.val < rseg->rsmp.val)
		err |= cpt_prepend(rseg,cpt);

	    /* the left */

	    lseg->rsmp.col.rgb = rgb; 
	    lseg->rsmp.val = SCALE(gseg->middle,opt);

	    grad_segment_colour(gseg->left,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);

	    lseg->lsmp.col.rgb = rgb; 
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
	    
	    rseg->rsmp.col.rgb = rgb; 
	    rseg->rsmp.val     = SCALE(x,opt);

	    x = gseg->right-width/n;
	    
	    grad_segment_colour(x,gseg,bg,col);
	    rgbD_to_rgb(col,&rgb);
	    
	    rseg->lsmp.col.rgb = rgb; 
	    rseg->lsmp.val     = SCALE(x,opt);
	    
	    cpt_prepend(rseg,cpt);
	    
	    for (i=2 ; i<=n ; i++)
	      {
		lseg = cpt_seg_new();
		
		lseg->rsmp.col.rgb = rseg->lsmp.col.rgb;
		lseg->rsmp.val     = rseg->lsmp.val;
		
		x = gseg->right-width*i/n;

		grad_segment_colour(x,gseg,bg,col);
		rgbD_to_rgb(col,&rgb);
		
		lseg->lsmp.col.rgb = rgb; 
		lseg->lsmp.val     = SCALE(x,opt);
		
		cpt_prepend(lseg,cpt);
		
		rseg = lseg;
	      }
	  }
	
	gseg = gseg->prev;
    } 
  
  return 0;
}

/*
  cpt_optimise tries to merge linear splines of the cpt
  path, and so reduce the size of the cpt file.
  it should, perhaps, be in common/cpt -- put it there 
  later
*/ 

/*
  this is where we convert the cpt to a path in 4-space,
  the rgb components and the path parameter. We'll need to
  test the scaling here: map everything into [0,1]^4 in
  the first instance
*/

static vertex_t smp_to_vertex(cpt_sample_t smp)
{
  vertex_t v;

#ifdef DEBUG
  int i;
#endif

  v.x[0] = smp.val;
  v.x[1] = smp.col.rgb.red/255.0; 
  v.x[2] = smp.col.rgb.green/255.0;
  v.x[3] = smp.col.rgb.blue/255.0; 

#ifdef DEBUG
  for (i=0 ; i<4 ; i++)
    printf("  %f",v.x[i]);
  printf("\n");
#endif

  return v;
}

static int cpt_optimise_segs(double tol,cpt_seg_t* seg,int len)
{
  int      k[len+1],i;
  vertex_t pv[len+1];
  cpt_seg_t* s;

  s = seg;

#ifdef DEBUG
  printf("length = %i\n",len);
#endif

  pv[0] = smp_to_vertex(s->lsmp);

  for (i=0 ; i<len ; i++)
    {
      pv[i+1] = smp_to_vertex(s->rsmp);
      s = s->rseg;
    }

  if (poly_simplify(tol,pv,len+1,k) != 0)
    {
      fprintf(stderr,"failed polyline simpification\n");
      return 1;
    }

  s = seg; 
  
  for (i=0 ; i<len ; i++)
    {
      if (k[i+1] == 0)
	{
	  cpt_seg_t *left,*right;

	  left  = s;
	  right = s->rseg;
	  
	  left->rsmp = right->rsmp;
	  left->rseg = right->rseg;

	  if (left->rseg)
	    left->rseg->lseg = left;
	    
	  cpt_seg_destroy(right);
	}
      else 
	s = s->rseg;
    }

  return 0;
}

static int cpt_optimise(double tol,cpt_t* cpt)
{
  int n,m;

  if ((n = cpt_nseg(cpt)) > 1)
    {
      int i,segos[n+1];

      m = cpt_npc(cpt,segos);

      if (m>0)
	{
	  cpt_seg_t *seg[m];
	  int        len[m];

	  segos[m] = n;

	  for (i=0 ; i<m ; i++)
	    {
	      if ((seg[i] = cpt_segment(cpt,segos[i])) == NULL)
		{
		  fprintf(stderr,"got null segment %i in section %i!\n",segos[i],i);
		  return 1;
		}
	      
	      len[i] = segos[i+1] - segos[i];
	    }

	  for (i=0 ; i<m ; i++)
	    {
	      if (cpt_optimise_segs(tol,seg[i],len[i]) != 0)
		{
		  fprintf(stderr,"failed optimise for section %i!\n",i);
		  return 1;
		}
	    }
	}
    }

  return 0;
}
    





