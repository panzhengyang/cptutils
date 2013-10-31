/*
  avlcpt.c
  convert arcview legend gradients to the cpt format
  (c) J.J. Green 2005, 2013
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cpt.h"
#include "cptwrite.h"
#include "cptname.h"
#include "avlgrad.h"

#include "avlcpt.h"

static int avlcpt_convert(avl_grad_t*, cpt_t*, avlcpt_opt_t);
static int avl_readfile(char*, avl_grad_t*, avlcpt_opt_t);

extern int avlcpt(avlcpt_opt_t opt)
{
  cpt_t* cpt;
  avl_grad_t avl;
    
  /* create a cpt struct */

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr, "failed to get new cpt strcture\n");
      return 1;
    }

  /* housekeeping */

  cpt->model = model_rgb;
  
  cpt->fg.type = cpt->bg.type = cpt->nan.type = fill_colour;

  cpt->bg.u.colour.rgb  = opt.bg;
  cpt->fg.u.colour.rgb  = opt.fg;
  cpt->nan.u.colour.rgb = opt.nan;

  /* transfer the gradient data to the cpt_t struct */

  if (avl_readfile(opt.file.input, &avl, opt) != 0)
    {
      fprintf(stderr, "failed to read data from %s\n", 
	      (opt.file.input ?  opt.file.input : "<stdin>"));
      return 1;
    }

  if (opt.file.input)
    cpt->name = cptname(opt.file.input, "avl");

  if (avlcpt_convert(&avl, cpt, opt) != 0)
    {
      fprintf(stderr, "failed to convert data\n");
      return 1;
    }

  avl_clean(&avl);

  if (opt.verbose) 
    {
      int n = cpt_nseg(cpt);
      printf("converted to %i segment rgb-spline\n", n);
    }
  
  /* write the cpt file */
  
  if (cpt_write(opt.file.output, cpt) != 0)
    {
      fprintf(stderr, "failed to write palette to %s\n", 
	      (opt.file.output ? opt.file.output : "<stdout>"));
      return 1;
    }
  
  /* tidy */
  
  cpt_destroy(cpt);
  
  return 0;
}

#define AVLCPT_INC   1
#define AVLCPT_DEC   2
#define AVLCPT_ERROR 3

static cpt_seg_t* cpt_seg_new_err(void);
static int cpt_append_err(cpt_seg_t*, cpt_t*);
static int avlcpt_direction(avl_grad_t*);

static int avlcpt_convert(avl_grad_t* avl, cpt_t *cpt, avlcpt_opt_t opt)
{
  int i, n, dir;
  double prec;

  if ((dir = avlcpt_direction(avl)) == AVLCPT_ERROR)
    {
      fprintf(stderr, "avl gradient direction error\n");
      return 1;
    }

  if (opt.verbose)
    printf("gradient is %s\n", 
	   (dir == AVLCPT_INC ? "increasing" : "decreasing"));
 
  prec = opt.precision;

  n = avl->n;

  for (i=0 ; i<n ; i++)
    {
      cpt_seg_t* cseg;
      avl_seg_t aseg = avl->seg[i];
      double z0, z1;

      if (aseg.nodata) continue;

      if (dir == AVLCPT_INC)
	{
	  z0 = aseg.min;
	  z1 = aseg.max;
	}
      else
	{
	  z0 = aseg.max;
	  z1 = aseg.min;
	}

      z0 = round(z0/prec)*prec;
      z1 = round(z1/prec)*prec;

      z0 -= opt.adjust.lower;
      z1 += opt.adjust.upper;

      if ((cseg = cpt_seg_new_err()) == NULL) return 1;

      cseg->lsmp.val = z0;
      cseg->lsmp.fill.type = fill_colour;
      cseg->lsmp.fill.u.colour.rgb.red   = aseg.r/256;
      cseg->lsmp.fill.u.colour.rgb.green = aseg.g/256;
      cseg->lsmp.fill.u.colour.rgb.blue  = aseg.b/256;
      
      cseg->rsmp.val = z1;
      cseg->rsmp.fill.type = fill_colour;
      cseg->rsmp.fill.u.colour.rgb.red   = aseg.r/256;
      cseg->rsmp.fill.u.colour.rgb.green = aseg.g/256;
      cseg->rsmp.fill.u.colour.rgb.blue  = aseg.b/256;
      
      if (cpt_append_err(cseg, cpt) != 0) return 1;
    }

  return 0;
}

/* 
   is the avlcpt gradient increasing or non-increasing?
   we also check that the gradients is monotone, else return
   error.
*/

static int avlcpt_direction(avl_grad_t* avl)
{
  int i, n, m;
  avl_seg_t* segs;

  n    = avl->n;
  segs = avl->seg;

  for (i=0, m=0 ; i<n ; i++)
    if (! segs[i].nodata) m++;

  if (m == 0)
    {
      fprintf(stderr, "gradient has no segments!\n");
      return AVLCPT_ERROR;
    }
  else if (m == 1)
    {
      fprintf(stderr, "gradient has at one segment!\n");
      return AVLCPT_INC;
    }
  else
    {
      /* extract the non-nodata min/max values first */

      double min[m], max[m];

      for (i=0, m=0 ; i<n ; i++)
	{	
	  if (! segs[i].nodata)
	    {
	      min[m] = segs[i].min;
	      max[m] = segs[i].max;
	      m++;
	    }
	}

      /* now monotonicity checking is easy */ 

      if (min[0] < min[1]) 
	{
	  for (i=1 ; i<m-1 ; i++)
	    {
	      if (! (min[i] < min[i+1]))
		{
		  fprintf(stderr, "increasing gradient started to decrease!\n");
		  fprintf(stderr, "(%.4f, %.4f) -> (%.4f, %.4f)\n", 
			  min[i], max[i], min[i+1], max[i+1]);
		  return AVLCPT_ERROR;
		}
	    }
	  return AVLCPT_INC;
	}
      else
	{
	  for (i=1 ; i<m-1 ; i++)
	    {
	      if (min[i] < min[i+1])
		{
		  fprintf(stderr, "decreasing gradient started to increase!\n");
		  fprintf(stderr, "(%.4f, %.4f) -> (%.4f, %.4f)\n", 
			  min[i], max[i], min[i+1], max[i+1]);
		  return AVLCPT_ERROR;
		}
	    }
	  return AVLCPT_DEC;
	}
    }

  return AVLCPT_ERROR;
}

/* error message versions of functions used in avlcpt_convert */

static cpt_seg_t* cpt_seg_new_err(void)
{
  cpt_seg_t* seg;

  if ((seg = cpt_seg_new()) == NULL) 
    {
      fprintf(stderr, "error creating segment\n");
      return NULL;
    }

  seg->annote = none;

  return seg;
}

static int cpt_append_err(cpt_seg_t* seg, cpt_t* cpt)
{
  if (cpt_append(seg, cpt) != 0)
    {
      fprintf(stderr, "error adding segment\n");
      return 1;
    }

  return 0;
}

/*
  handle stream choice and call libavl function, read_avl()
*/ 

static int avl_readfile(char* file, avl_grad_t* avl, avlcpt_opt_t opt)
{
  int err = 0;

  if (file)
    {
      FILE* s;

      if ((s = fopen(file, "r")) == NULL)
	{
	  fprintf(stderr, "failed to open %s\n", file);
	  return 1;
	}

      err = avl_read(s, avl, opt.verbose, opt.debug);

      fclose(s);
    }
  else
    err = avl_read(stdin, avl, opt.verbose, opt.debug);

  return err;
}

