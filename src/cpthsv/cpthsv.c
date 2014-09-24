/*
  cpthsv.c

  (c) J.J.Green 2007
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cptread.h>
#include <cptwrite.h>
#include <btrace.h>

#include "cpthsv.h"

static int cpthsv_convert(cpt_t*,cpthsv_opt_t);

extern int cpthsv(char* infile,char* outfile,cpthsv_opt_t opt)
{
  cpt_t *cpt;
  int err=0;

  if ((cpt = cpt_new()) != NULL)
    {
      if (cpt_read(infile, cpt) == 0)
	{
	  if (cpthsv_convert(cpt, opt) == 0)
	    {
	      if (cpt_write(outfile, cpt) == 0)
		{
		  /* success */
		}
	      else
		{
		  btrace("error writing cpt struct");
		  err = 1;
		}
	    }
	  else
	    {
	      btrace("failed to convert");
	      err = 1;
	    }
	}
      else
	{
	  btrace("failed to load cpt from %s",
		     (infile ? infile : "<stdin>"));
	  err = 1;
	}
      
      cpt_destroy(cpt);    
    }	
  else
    {
      btrace("failed to create cpt struct");
      err = 1;
    }
  
  if (err)
    btrace("failed to write cpt to %s", (outfile ? outfile : "<stdout>"));
  
  return err;
}

static void rgb_transform(rgb_t*,cpthsv_opt_t);

static int cpthsv_convert(cpt_t* cpt,cpthsv_opt_t opt)
{
  /* check we have at least one cpt segment */

  if (cpt->segment == NULL)
    {
      btrace("cpt has no segments");
      return 1;
    }

  /* check colour model */

  if (cpt->model != model_rgb)
    {
      btrace("cannot process non-rgb palette, sorry");
      return 1;
    }

  /* convert cpt segments */

  cpt_seg_t *seg;
  int n,m;

  for (n=0,m=0,seg=cpt->segment ; seg ; seg=seg->rseg,n++)
    {
      if (seg->lsmp.fill.type != seg->rsmp.fill.type)
        {
          btrace("sorry, can't convert mixed fill types");
          return 1;
        }

      if (seg->lsmp.fill.type == fill_colour)
	{
	  m++;

	  rgb_transform(&(seg->lsmp.fill.u.colour.rgb),opt);
	  rgb_transform(&(seg->rsmp.fill.u.colour.rgb),opt);
	}
    }

  if (opt.verbose)
    printf("transformed %i/%i segments\n",m,n);
  
  return 0;
}

static void hsv_transform(hsv_t*,hsvtrans_t);

static void rgb_transform(rgb_t* rgb,cpthsv_opt_t opt)
{
  double rgbD[3];
  hsv_t hsv; 

  rgb_to_rgbD(*rgb,rgbD);
  rgbD_to_hsv(rgbD,&hsv);

  int i;

  for (i=0 ; i<opt.n ; i++)
    hsv_transform(&hsv,opt.tran[i]);

  hsv_to_rgbD(hsv,rgbD);
  rgbD_to_rgb(rgbD,rgb);
}

static void double_transform(double*,hsvtrans_t);
static double norm1(double);
static double norm360(double);

static void hsv_transform(hsv_t* hsv,hsvtrans_t t)
{
  switch (t.channel)
    {
    case hue:
      double_transform(&(hsv->hue),t);
      hsv->hue = norm360(hsv->hue);
      break;
    case saturation:
      double_transform(&(hsv->sat),t);
      hsv->sat = norm1(hsv->sat);
      break;
    case value:
      double_transform(&(hsv->val),t);
      hsv->val = norm1(hsv->val);
      break;
    }
}

static void double_transform(double* x,hsvtrans_t t)
{
  switch (t.op)
    {
    case times:   *x *= t.z; break;
    case plus:    *x += t.z; break;
    case minus:   *x -= t.z; break;
    case percent: *x *= t.z/100.0; break;
    }
}

static double norm1(double a)
{
  a = (a<0 ? 0 : a);
  a = (a>1 ? 1 : a);

  return a;
}

static double norm360(double a)
{
  while (a>360) a -= 360;
  while (a<0) a += 360;

  return a;
}




