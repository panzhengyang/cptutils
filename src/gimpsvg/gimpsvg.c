/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimpcpt.c,v 1.6 2004/02/19 00:50:23 jjg Exp jjg $
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

#define  SCALE(x,opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

static int gradcpt(gradient_t*,cpt_t*,cptopt_t);
static int cpt_optimise(double,cpt_t*);
static int cpt_nseg(cpt_t*);

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

    /* perform optimisation */

    if (opt.verbose) 
      {
	int n = cpt_nseg(cpt);
	printf("transformed %i segment%s\n",n,(n-1 ? "s" : ""));
      }

    if (cpt_optimise(0.5,cpt) != 0)
      {
	fprintf(stderr,"failed to optimise cpt\n");
	return 1;
      }

    if (opt.verbose) 
      {
	int n = cpt_nseg(cpt);
	printf("optimised to %i segment%s\n",n,(n-1 ? "s" : ""));
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

static cpt_seg_t* cpt_segment(cpt_t* cpt,int n)
{
  cpt_seg_t *seg;

  seg = cpt->segment;

  while (--n)
    {
      if ((seg = seg->rseg) == NULL) 
	return NULL; 
    }

  return seg;
}

static int cpt_nseg(cpt_t* cpt)
{
  int n = 0;
  cpt_seg_t *seg;

  for (seg = cpt->segment ; seg ; seg = seg->rseg)
    n++;

  return n;
}

static double colour_rgb_dist(colour_t a,colour_t b,model_t model)
{
  double da[3],db[3],sum;
  int i;

  switch (model)
    {

    case rgb :
      rgb_to_rgbD(a.rgb,da); 
      rgb_to_rgbD(b.rgb,db); 
      break;
      
    case hsv :
      hsv_to_rgbD(a.hsv,da); 
      hsv_to_rgbD(b.hsv,db); 
      break;

    default:

      return -1.0;
    }

  for (sum=0.0,i=0 ; i<3 ; i++)
    {
      double d;

      d = da[i]-db[i];
      sum += d*d;
    }

  return sqrt(sum);
}

static int cpt_npc(cpt_t* cpt,int *segos)
{
  int i,n;
  cpt_seg_t *left,*right;
  double tol = 1e-6;

  left=cpt->segment;

  if (! left) return 0;

  segos[0] = 0;

  right = left->rseg;

  if (! right) return 1;

  n = 1;
  i = 0;

  while (right)
    {    
      if (colour_rgb_dist(
			  left->rsmp.col,
			  right->lsmp.col,
			  cpt->model
			  ) > tol)
	{
	  segos[n] = i;
	  n++;
	}

      left  = right;
      right = left->rseg;
      i++;
    }

  return n;
}

static vertex_t smp_to_vertex(cpt_sample_t smp)
{
  vertex_t v;
  
  v.x[0] = smp.col.rgb.red; 
  v.x[1] = smp.col.rgb.green;
  v.x[2] = smp.col.rgb.blue; 
  
  return v;
}

static int cpt_optimise_segs(double tol,cpt_seg_t* seg,int len)
{
  int      k[len+1],i;
  vertex_t pv[len+1];
  cpt_seg_t* s;

  s = seg;

  pv[0] = smp_to_vertex(s->lsmp);

  for (i=0 ; i<len ; i++)
    {
      pv[i+1] = smp_to_vertex(s->rsmp);
      s = s->rseg;
    }

  if (poly_simplify(tol,pv,len+1,k) != 0)
    return 1;

  s = seg->rseg; 
  
  for (i=0 ; i<len-1 ; i++)
    {
      if (k[i+1] == 0)
	{
	  cpt_seg_t *left,*right;

	  left  = s->lseg;
	  right = s->rseg;
	  
	  left->rseg  = right;
	  right->lseg = left;

	  left->rsmp = right->lsmp;
	    
	  cpt_seg_destroy(s);

	  s = left;
	}

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

      segos[m] = n;

      for (i=0 ; i<m ; i++)
	{
	  int os  = segos[i];
	  int len = segos[i+1] - segos[i];
	  cpt_seg_t *seg;

	  /* FIXME */

	  if ((seg = cpt_segment(cpt,os)) == NULL) return 1;
	  if (cpt_optimise_segs(tol,seg,len) != 0) return 1;
	}
    }

  return 0;
}
    





