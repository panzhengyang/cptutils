/*
  pspsvg.c

  convert paintshop pro gradients to the svg format

  (c) J.J. Green 2005,2006
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pspread.h"
#include "svgwrite.h"

#include "pspsvg.h"
#include "grdxsvg.h"
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

  if (psp_read(opt.file.input, psp) != 0)
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
  
  if (svg_write(opt.file.output, 1, (const svg_t**)(&svg), &(opt.preview)) != 0)
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

/* convert psp to intermediate types */

static double psp_rgb_it(unsigned short x)
{
  return (double)x/65535.0;
}

static double psp_op_it(unsigned short x)
{
  return (double)x/255.0;
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


/* trim off excessive stops */

static int trim_rgb(gstack_t* stack)
{
  int n = gstack_size(stack);
  rgb_stop_t stop;
  gstack_t *stack0;

  if ((stack0 = gstack_new(sizeof(rgb_stop_t), n, n)) == NULL)
    return 1;

  while (! gstack_empty(stack))
    {
      gstack_pop(stack, &stop);
      gstack_push(stack0, &stop);

      if (stop.z >= 409600)
	{
	  while (! gstack_empty(stack))
	    gstack_pop(stack,&stop);
	}
    }

  while (! gstack_empty(stack0))
    {
      gstack_pop(stack0,&stop);
      gstack_push(stack,&stop);
    }

  return 0;
}

static int trim_op(gstack_t* stack)
{
  int n = gstack_size(stack);
  op_stop_t stop;
  gstack_t *stack0;

  if ((stack0 = gstack_new(sizeof(op_stop_t), n, n)) == NULL)
    return 1;

  while (! gstack_empty(stack))
    {
      gstack_pop(stack, &stop);
      gstack_push(stack0, &stop);

      if (stop.z >= 409600)
	{
	  while (! gstack_empty(stack))
	    gstack_pop(stack,&stop);
	}
    }

  while (! gstack_empty(stack0))
    {
      gstack_pop(stack0,&stop);
      gstack_push(stack,&stop);
    }

  return 0;
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
      fprintf(stderr,"input (grd) has %i rgb stop(s)\n",n);
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

  /* add implicit final stop */

  if (stop.z < 409600)
    {
      stop.z = 409600;

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  if (gstack_reverse(stack) != 0)
    return NULL;

  if (trim_rgb(stack) != 0)
    return NULL;

  return stack;
}

static gstack_t* rectify_op(psp_t* psp)
{
  psp_opseg_t *pseg = psp->op.seg;
  int i,n = psp->op.n;

  if (n<2)
    {
      fprintf(stderr,"input (grd) has %i opacity stop(s)\n",n);
      return NULL;
    }

  gstack_t *stack;

  if ((stack = gstack_new(sizeof(op_stop_t), 2*n, n)) == NULL)
    return NULL;

  op_stop_t stop;

  if (pseg[0].z > 0)
    {
      stop.z  = 0;
      stop.op = psp_op_it(pseg[0].opacity);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  for (i=0 ; i<n-1 ; i++)
    {
      stop.z  = psp_z_it(pseg[i].z);
      stop.op = psp_op_it(pseg[i].opacity);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
      
      if (pseg[i].midpoint != 50)
	{
	  stop.z  = psp_zmid_it(pseg[i].z, pseg[i+1].z, pseg[i].midpoint);
	  stop.op = 0.5*(psp_op_it(pseg[i].opacity) + 
			 psp_op_it(pseg[i+1].opacity));

	  if (gstack_push(stack, &stop) != 0)
	    return NULL;
	}
    }

  stop.z  = psp_z_it(pseg[n-1].z);
  stop.op = psp_op_it(pseg[n-1].opacity);

  if (gstack_push(stack, &stop) != 0)
    return NULL;

  if (stop.z < 409600)
    {
      stop.z = 409600;
      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  if (gstack_reverse(stack) != 0)
    return NULL;

  if (trim_op(stack) != 0)
    return NULL;

  return stack;
}

/* 
   In grd3 files, names are non null-terminated
   arrays of unsigned char, but we store them as
   null-terminated arrays of unsigned char.  In
   the wild one sees the upper half of the range
   being used, and it seems to be latin-1.  

   SVG uses utf8, so we need to convert our latin-1 
   to it: the implementation taken from 
   http://stackoverflow.com/questions/4059775 
*/

static int latin1_to_utf8(const unsigned char *in, 
			  unsigned char *out,
			  size_t lenout)
{
  size_t lenin = 0;
  const unsigned char* p;

  for (p=in ; *p ; p++) lenin++;

  if (2*lenin >= lenout)
    return 1;

  while (*in)
    {
      if (*in < 128) 
	*out++ = *in++;
      else if (*in < 192)
	return 1;
      else 
	{
	  *out++ = 0xc2 + (*in > 0xbf);
	  *out++ = (*in++ & 0x3f) + 0x80;
	}
    }

  return 0;
}

static int pspsvg_convert(psp_t *psp, svg_t *svg, pspsvg_opt_t opt)
{
  gstack_t *rgbrec,*oprec;

  if (latin1_to_utf8(psp->name, svg->name, SVG_NAME_LEN) != 0)
    {
      fprintf(stderr, "failed latin1 to unicode name conversion\n");
      return 1;
    }

  if (opt.verbose)
    printf("processing \"%s\"\n", svg->name);

  if ((rgbrec = rectify_rgb(psp)) == NULL)
    return 1;

  if ((oprec = rectify_op(psp)) == NULL)
    return 1;

  if (opt.verbose)
    printf("stops: rgb %i/%i, opacity %i/%i\n",
	   psp->rgb.n,
	   gstack_size(rgbrec),
	   psp->op.n,
	   gstack_size(oprec));

  if (grdxsvg(rgbrec, oprec, svg) != 0)
    {
      fprintf(stderr, "failed conversion of rectified stops to svg\n");
      return 1;
    }
  
  gstack_destroy(rgbrec);
  gstack_destroy(oprec);

  return 0;
}




