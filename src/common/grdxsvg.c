/*
  grdxsvg.c

  Copyright (c) J.J. Green 2014
*/

#include <stdio.h>

#include "grdxsvg.h"

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

  if ((rs0.z != 0) || (os0.z != 0))
    {
      fprintf(stderr,"nonzero initial opacity %i %i\n",os0.z,os1.z);
      return NULL;
    }

  /* merged stack to return */

  if ((ross = gstack_new(sizeof(rgbop_stop_t), n, n)) == NULL)
    return NULL;

  rgbop_stop_t ros;

  while (1)
    {
      ros = stop_merge(rs0, os0);
      gstack_push(ross, &ros);

      if (rs1.z > os1.z)
	{
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
#define DEBUG

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


extern int grdxsvg(gstack_t *rgb_stops,
		   gstack_t *op_stops,
		   svg_t *svg)
{
  gstack_t *merged_stops;
    
  if ((merged_stops = merge(rgb_stops, op_stops)) == NULL)
    return 1;

  if (merged_svg(merged_stops, svg) != 0)
    return 1;

  return 0;
}
