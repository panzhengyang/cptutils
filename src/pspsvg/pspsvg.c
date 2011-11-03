/*
  pspsvg.c

  convert paintshop pro gradients to the svg format

  (c) J.J. Green 2005,2006
  $Id: pspsvg.c,v 1.5 2011/11/02 20:36:51 jjg Exp jjg $
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

/* convert intermediate types to svg values */

static unsigned char svg_it_rgb(double x)
{
  x *= 256;
  if (x > 255) x = 255;
  if (x < 0) x = 0;

  return x;
}

static double svg_it_op(double x)
{
  return x;
}

static double svg_it_z(double x)
{
  return x  / 4096.0;
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
      if (pseg[i].z >= 4096) break;

      stop.z = psp_z_it(pseg[i].z);
      stop.r = psp_rgb_it(pseg[i].r);
      stop.g = psp_rgb_it(pseg[i].g);
      stop.b = psp_rgb_it(pseg[i].b);

      printf("%i %i %i %i\n",pseg[i].z,pseg[i].r,pseg[i].g,pseg[i].b);

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

  stop.z = psp_z_it(pseg[i].z);
  stop.r = psp_rgb_it(pseg[i].r);
  stop.g = psp_rgb_it(pseg[i].g);
  stop.b = psp_rgb_it(pseg[i].b);
  
  printf("%i %i %i %i\n",pseg[i].z,pseg[i].r,pseg[i].g,pseg[i].b);

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

      printf("%i %i (implicit)\n",0,pseg[0].opacity);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  for (i=0 ; i<n-1 ; i++)
    {
      if (pseg[i].z >= 4096) break;

      stop.z  = psp_z_it(pseg[i].z);
      stop.op = psp_op_it(pseg[i].opacity);

      printf("%i %i\n",pseg[i].z,pseg[i].opacity);

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

  stop.z  = psp_z_it(pseg[i].z);
  stop.op = psp_op_it(pseg[i].opacity);
  
  printf("%i %i\n",pseg[i].z,pseg[i].opacity);

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

  return stack;
}

static rgbop_stop_t stop_merge(rgb_stop_t rs, op_stop_t os)
{
  rgbop_stop_t ros;

  ros.r  = rs.r;
  ros.g  = rs.g;
  ros.b  = rs.b;
  ros.op = os.op;
  ros.z  = rs.z;

  return ros;
}

static rgb_stop_t rgb_stop_interp(rgb_stop_t rs0,
				  rgb_stop_t rs1, 
				  unsigned int z)
{
  double M = 
    ((double)z - (double)(rs0.z))/((double)(rs1.z) - (double)(rs0.z));
  rgb_stop_t rs;

  rs.z = z;
  rs.r = rs0.r + M*(rs1.r - rs0.r);
  rs.g = rs0.g + M*(rs1.g - rs0.g);
  rs.b = rs0.b + M*(rs1.b - rs0.b);
  
  return rs;
}

static op_stop_t op_stop_interp(op_stop_t os0,
				op_stop_t os1, 
				unsigned int z)
{
  double M = 
    ((double)z -(double)(os0.z))/((double)(os1.z) - (double)(os0.z));
  op_stop_t os;

  os.z = z;
  os.op = os0.op + M*(os1.op - os0.op);

  return os;
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

  /* get the first two of each type of stop */

  rgb_stop_t rs0, rs1;
  
  err += gstack_pop(rss, &rs0);
  err += gstack_pop(rss, &rs1);

  if (err)
    {
      fprintf(stderr,"%i errors rgb\n",err);
      return NULL;
    }

  op_stop_t os0, os1;
  
  err += gstack_pop(oss, &os0);
  err += gstack_pop(oss, &os1);

  if (err)
    {
      fprintf(stderr,"%i errors rgb\n",err);
      return NULL;
    }

  printf("OK\n");

  if ((rs0.z != 0) || (os0.z != 0))
    {
      fprintf(stderr,"nonzero initial opacity %i %i\n",os0.z,os1.z);
      return NULL;
    }

  printf("OK\n");

  /* merged stack to return */

  if ((ross = gstack_new(sizeof(rgbop_stop_t), n, n)) == NULL)
    return NULL;

  rgbop_stop_t ros;

  while (1)
    {
      ros = stop_merge(rs0, os0);
      gstack_push(ross, &ros);

      fprintf(stderr,"(%i %i) (%i %i)\n",rs0.z,rs1.z,os0.z,os1.z);

      if (rs1.z > os1.z)
	{
	  printf("interp rgb\n");

	  rs0 = rgb_stop_interp(rs0, rs1, os1.z);
	  os0 = os1;

	  if (gstack_pop(oss, &os1) != 0)
	    {
	      fprintf(stderr,"early termination of opacity channel\n");
	      break;
	    }
	}
      else if (rs1.z < os1.z)
	{
	  printf("interp op\n");

	  os0 = op_stop_interp(os0, os1, rs1.z);
	  rs0 = rs1;

	  if (gstack_pop(rss, &rs1) != 0)
	    {
	      fprintf(stderr,"early termination of rgb channel\n");
	      break;
	    }
	}
      else
	{
	  rs0 = rs1;
	  os0 = os1;

	  int 
            odone = gstack_pop(oss, &os1),
            rdone = gstack_pop(rss, &rs1);

	  if (odone && rdone)
	    {
	      ros = stop_merge(rs0, os0);
	      gstack_push(ross, &ros);
	      gstack_reverse(ross);

	      return ross;
	    }
	  else if (odone && !rdone)
	    {
	      fprintf(stderr,"early termination of opacity channel\n");
	      break;
	    }
	  else if (rdone && ! odone)
	    {
	      fprintf(stderr,"early termination of rgb channel\n");
	      break;
	    }
	  else
	    {
	      /* OK, so now we continue */
	    }
	}
    }

  /* something has gone pear-shaped */

  gstack_destroy(ross);

  return NULL;
}

static int merged_svg(gstack_t *ross, svg_t *svg)
{
  svg_stop_t ss;
  rgbop_stop_t ros;

  while (gstack_pop(ross,&ros) == 0)
    {
      ss.colour.red   = svg_it_rgb(ros.r);
      ss.colour.green = svg_it_rgb(ros.g);
      ss.colour.blue  = svg_it_rgb(ros.b);
      ss.opacity      = svg_it_op(ros.op);
      ss.value        = svg_it_z(ros.z);

#define DEBUG

#ifdef DEBUG
      
      printf("%7.3f %3i %3i %3i %f\n",
	     ss.value,
	     ss.colour.red,
	     ss.colour.green,
	     ss.colour.blue,
	     ss.opacity
	     );

#endif

      svg_append(ss,svg);
    }

  return 0;
}

static int pspsvg_convert(psp_t *psp, svg_t *svg, pspsvg_opt_t opt)
{
  gstack_t *rgbrec,*oprec;

  if (opt.verbose)
    printf("processing \"%s\"\n",psp->name);

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

  gstack_t *m;

  if ((m = merge(rgbrec,oprec)) == NULL)
    return 1;

  if (opt.verbose)
    printf("merged to %i\n", gstack_size(m));
  
  gstack_destroy(rgbrec);
  gstack_destroy(oprec);

  if (merged_svg(m,svg) != 0)
    return 1;

  gstack_destroy(m);

  if (snprintf(svg->name, SVG_NAME_LEN, "%s", psp->name) >= SVG_NAME_LEN)
    fprintf(stderr, "truncated svg name!\n");

  return 0;
}




