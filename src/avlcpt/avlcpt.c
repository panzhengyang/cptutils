/*
  avlcpt.c

  convert arcview legend gradients to the cpt format

  (c) J.J. Green 2005
  $Id: avlcpt.c,v 1.1 2005/11/13 23:49:51 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cpt.h"
#include "cptio.h"
#include "avl.h"

#include "avlcpt.h"

static int avlcpt_convert(avl_grad_t*,cpt_t*,avlcpt_opt_t);
static int avl_readfile(char*,avl_grad_t*,avlcpt_opt_t);

extern int avlcpt(avlcpt_opt_t opt)
{
  cpt_t* cpt;
  avl_grad_t avl;
    
  /* create a cpt struct */

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr,"failed to get new cpt strcture\n");
      return 1;
    }

  cpt->model = rgb;
  
  cpt->fg.type = cpt->bg.type = cpt->nan.type = colour;

  /* transfer the gradient data to the cpt_t struct */

  if (avl_readfile(opt.file.input,&avl,opt) != 0)
    {
      fprintf(stderr,"failed to read data from %s\n",
	      (opt.file.input ?  opt.file.input : "<stdin>"));
      return 1;
    }

  // causes a core dump
  // strncpy(cpt->name,avl.name,CPT_NAME_LEN);

  if (avlcpt_convert(&avl,cpt,opt) != 0)
    {
      fprintf(stderr,"failed to convert data\n");
      return 1;
    }

  avl_clean(&avl);

  if (opt.verbose) 
    {
      int n = cpt_nseg(cpt);
      printf("converted to %i segment rgb-spline\n",n);
    }
  
  /* write the cpt file */
  
  if (cpt_write(opt.file.output,cpt) != 0)
    {
      fprintf(stderr,"failed to write palette to %s\n",
	      (opt.file.output ? opt.file.output : "<stdout>"));
      return 1;
    }
  
  /* tidy */
  
  cpt_destroy(cpt);
  
  return 0;
}

static cpt_seg_t* cpt_seg_new_err(void);
static int cpt_append_err(cpt_seg_t*,cpt_t*);

static int avlcpt_convert(avl_grad_t* avl,cpt_t *cpt,avlcpt_opt_t opt)
{
  return 0;
}

/* error message versions of functions used in avlcpt_convert */

static cpt_seg_t* cpt_seg_new_err(void)
{
  cpt_seg_t* seg;

  if ((seg = cpt_seg_new()) == NULL) 
    {
      fprintf(stderr,"error creating segment\n");
      return NULL;
    }

  seg->annote = none;

  return seg;
}

static int cpt_append_err(cpt_seg_t* seg,cpt_t* cpt)
{
  if (cpt_append(seg,cpt) != 0)
    {
      fprintf(stderr,"error adding segment\n");
      return 1;
    }

  return 0;
}

/*
  handle stream choice and call libavl function, read_avl()
*/ 

static int avl_readfile(char* file,avl_grad_t* avl,avlcpt_opt_t opt)
{
  int err = 0;

  if (file)
    {
      FILE* s;

      if ((s = fopen(file,"r")) == NULL)
	{
	  fprintf(stderr,"failed to open %s\n",file);
	  return 1;
	}

      err = avl_read(s,avl,opt.verbose,opt.debug);

      fclose(s);
    }
  else
    err = avl_read(stdin,avl,opt.verbose,opt.debug);

  return err;
}

