/*
  pspsvg.c

  convert paintshop pro gradients to the svg format

  (c) J.J. Green 2005,2006
  $Id: pspsvg.c,v 1.11 2009/10/08 17:39:24 jjg Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pspread.h"
#include "svgwrite.h"

#include "pspsvg.h"
#include "gstack.h"

static int pspsvg_convert(psp_t*, svg_t*, pspsvg_opt_t);

extern int pspsvg(pspsvg_opt_t opt)
{
  svg_t* svg;
  psp_t* psp;
    
  /* create & intialise a svg struct */

  if ((svg = svg_new()) == NULL)
    {
      fprintf(stderr,"failed to get new svg strcture\n");
      return 1;
    }

  /* create & read psp */

  if ((psp = psp_new()) == NULL)
    {
      fprintf(stderr,"failed to get new psp struture\n");
      return 1;
    }

  if (psp_read(opt.file.input,psp) != 0)
    {
      fprintf(stderr,"failed to read data from %s\n",
	      (opt.file.input ?  opt.file.input : "<stdin>"));
      return 1;
    }
  
  /* convert */

  if (pspsvg_convert(psp, svg, opt) != 0)
    {
      fprintf(stderr,"failed to convert data\n");
      return 1;
    }
  
  /* write the svg file */
  
  if (svg_write(opt.file.output,svg) != 0)
    {
      fprintf(stderr,"failed to write palette to %s\n",
	      (opt.file.output ? opt.file.output : "<stdout>"));
      return 1;
    }
  
  /* tidy */

  psp_destroy(psp);  
  svg_destroy(svg);
  
  return 0;
}

/* intermediate types */

typedef struct 
{
  unsigned int z;
  double r,g,b;
} rgb_stop_t;

typedef struct 
{
  unsigned int z;
  double op;
} op_stop_t;

typedef struct 
{
  unsigned int z;
  double r,g,b,op;
} rgbop_stop_t;

/* convert to intermediate types */

static double psp_rgb_it(unsigned short x)
{
  return (double)x/65535.0;
}

static double psp_op_it(unsigned short x)
{
  return (double)x/256.0;
}

static unsigned int psp_z_it(unsigned short z)
{
  return (unsigned int)z*100;
}

static unsigned int psp_zmid_it(unsigned short z0, 
				unsigned short z1, 
				unsigned short M)
{
  return (unsigned int)z0*100 + ((unsigned int)z1 - (unsigned int)z0)*M;
}

/* 
   convert the psp stops to the intermediate types, 
   and rectify -- replace the midpoints by explicit 
   mid-point stops
*/

static gstack_t* rectify_rgb(psp_t* psp)
{
  psp_rgbseg_t *pseg = psp->rgb.seg;
  int i,n = psp->rgb.n;

  if (n<2)
    {
      fprintf(stderr,"there is not enough data to make a palette!\n");
      return NULL;
    }

  gstack_t *stack;

  if ((stack = gstack_new(sizeof(rgb_stop_t), 2*n, n)) == NULL)
    return NULL;

  rgb_stop_t stop;

  for (i=0 ; i<n-1 ; i++)
    {
      stop.z = psp_z_it(pseg[i].z);
      stop.r = psp_rgb_it(pseg[i].r);
      stop.g = psp_rgb_it(pseg[i].g);
      stop.b = psp_rgb_it(pseg[i].b);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
      
      if (pseg[i].midpoint != 50)
	{
	  stop.z = psp_zmid_it(pseg[i].z, pseg[i+1].z, pseg[i].midpoint);
	  stop.r = 0.5*(psp_rgb_it(pseg[i].r) + psp_rgb_it(pseg[i+1].r));
	  stop.g = 0.5*(psp_rgb_it(pseg[i].g) + psp_rgb_it(pseg[i+1].g));
	  stop.b = 0.5*(psp_rgb_it(pseg[i].b) + psp_rgb_it(pseg[i+1].b));

	  if (gstack_push(stack, &stop) != 0)
	    return NULL;
	}
    }

  stop.z = psp_z_it(pseg[n-1].z);
  stop.r = psp_rgb_it(pseg[n-1].r);
  stop.g = psp_rgb_it(pseg[n-1].g);
  stop.b = psp_rgb_it(pseg[n-1].b);
  
  if (gstack_push(stack, &stop) != 0)
    return NULL;

  if (gstack_reverse(stack) != 0)
    return NULL;

  return stack;
}

static gstack_t* rectify_op(psp_t* psp)
{
  psp_opseg_t *pseg = psp->op.seg;
  int i,n = psp->op.n;

  if (n<2)
    {
      fprintf(stderr,"there is not enough data to make a palette!\n");
      return NULL;
    }

  gstack_t *stack;

  if ((stack = gstack_new(sizeof(op_stop_t), 2*n, n)) == NULL)
    return NULL;

  op_stop_t stop;

  for (i=0 ; i<n-1 ; i++)
    {
      stop.z  = psp_z_it(pseg[i].z);
      stop.op = psp_op_it(pseg[i].opacity);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
      
      if (pseg[i].midpoint != 50)
	{
	  stop.z  = psp_zmid_it(pseg[i].z, pseg[i+1].z, pseg[i].midpoint);
	  stop.op = 0.5*(psp_op_it(pseg[i].opacity) + psp_op_it(pseg[i+1].opacity));

	  if (gstack_push(stack, &stop) != 0)
	    return NULL;
	}
    }

  stop.z  = psp_z_it(pseg[n-1].z);
  stop.op = psp_op_it(pseg[n-1].opacity);
  
  if (gstack_push(stack, &stop) != 0)
    return NULL;

  if (gstack_reverse(stack) != 0)
    return NULL;

  return stack;
}

/* 
   merge the independent rgb and opacity channels into
   a single rgbop channel. This means we interpolate 
   each of the channels at the z-values of the other
*/

static gstack_t* merge(gstack_t *rss, gstack_t *oss)
{
  gstack_t *ross;
  int err = 0, n = gstack_size(rss) + gstack_size(oss);

  if ((ross = gstack_new(sizeof(rgbop_stop_t), n, n)) == NULL)
    return NULL;

  /* get the first two of each type of stop */

  rgb_stop_t rs0,rs1;
  
  err += gstack_pop(rss,&rs0);
  err += gstack_pop(rss,&rs1);

  if (err) return NULL;

  rgb_stop_t os0,os1;
  
  err += gstack_pop(oss,&os0);
  err += gstack_pop(oss,&os1);

  if (err) return NULL;

  printf("%u %u\n", rs0.z, rs1.z);
  printf("%u %u\n", os0.z, os1.z);

  // to finish

  return ross;
}

static int pspsvg_convert(psp_t *psp, svg_t *svg, pspsvg_opt_t opt)
{
  gstack_t *rgbrec,*oprec;

  if ((rgbrec = rectify_rgb(psp)) == NULL)
    return 1;

  if ((oprec = rectify_op(psp)) == NULL)
    return 1;

  gstack_t *m;

  if ((m = merge(rgbrec,oprec)) == NULL)
    return 1;
  
  gstack_destroy(rgbrec);
  gstack_destroy(oprec);

  // convert m to svg

  gstack_destroy(m);

  return 0;
}




