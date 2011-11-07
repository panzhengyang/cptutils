/*
  colours.c
  
  colours for gimpcpt

  (c) J.J.Green 2001,2004
  $Id: colour.c,v 1.7 2010/04/04 17:22:15 jjg Exp jjg $
*/

#define _GNU_SOURCE

#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)>(b)) ? (b) : (a))

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "colour.h"

static double colourD(int);
static int colour8(double);

extern double colour_rgb_dist(colour_t a,colour_t b,model_t model)
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

/*
  convert a gimp triple of [0..1] vals to/from an rgb_t struct
*/

extern int rgbD_to_rgb(double* col,rgb_t* rgb)
{
    if (!col || !rgb) return 1;

    rgb->red   = colour8(col[0]);
    rgb->green = colour8(col[1]);
    rgb->blue  = colour8(col[2]);

    return 0;
}

extern int rgb_to_rgbD(rgb_t rgb,double* col)
{
    if (!col) return 1;

    col[0] = colourD(rgb.red);
    col[1] = colourD(rgb.green);
    col[2] = colourD(rgb.blue);

    return 0;
}

extern int grey_to_rgbD(int g,double* col)
{
  rgb_t rgb;

  rgb.red   = g;
  rgb.green = g;
  rgb.blue  = g;

  return rgb_to_rgbD(rgb,col);
}

/*
  convert gimp rgbD to/from gmt hsv type -- this is
  needed since gmt stores its hsv format as hsv triples,
  whereas gimp converts them to rgb, reflecting the format's
  purpose  : the former is easier for hand-crafting, the
  latter for automated processing of GUI generation.
*/

extern int rgbD_to_hsv(double* rgbD,hsv_t* hsv)
{
  double hsvD[3];

  if (!rgbD || !hsv) return 1;
  
  if (rgbD_to_hsvD(rgbD,hsvD) != 0)
    return 1;
  
  hsv->hue = hsvD[0] * 360.0;
  hsv->sat = hsvD[1];
  hsv->val = hsvD[2];

  return 0;
}

extern int hsv_to_rgbD(hsv_t hsv,double* rgbD)
{
  double hsvD[3];
  
  hsvD[0] = hsv.hue/360.0;
  hsvD[1] = hsv.sat;
  hsvD[2] = hsv.val;

  if (!rgbD) return 1;

  if (hsvD_to_rgbD(hsvD,rgbD) != 0)
    return 1;
  
  return 0;
}

/*
  convert a number in [0,1] to/from something in 0...255 
*/

static int colour8(double x)
{
    int i;

    i = nearbyint(255*x);

    i = MAX(i,0);
    i = MIN(i,255);

    return i;
}

static double colourD(int i)
{
    double x;

    x = i/255.0;

    x = (x<0.0 ? 0.0 : x);
    x = (x>1.0 ? 1.0 : x);

    return x;
}
    
/*
  read an rgb_t in a string "r/g/b/"
*/ 

extern int parse_rgb(char* string,rgb_t* col)
{
    char *dup,*token;
   
    if (string == NULL) return 1;
    if ((dup = strdup(string)) == NULL) return 1;
 
    token = strtok(dup,"/");

    col->red = atoi(token);

    if ((token = strtok(NULL,"/")) == NULL)
    {
	col->blue = col->green = col->red;
	free(dup);
	return 0;
    }

    col->green = atoi(token);

    if ((token = strtok(NULL,"/")) == NULL)
    {
	fprintf(stderr,"ill-formed colour string \"%s\"\n",string);
	free(dup);
	return 1;
    }
    
    col->blue = atoi(token);
    free(dup);

    return 0;
}

/*
  taken from libgimp/gimpcolorspace.c with minor modifications
*/

extern int rgbD_to_hsvD(double *rgbD,double* hsvD)
{
    double min, max;
    double delta,r,g,b,h,s,v;
    
    r = rgbD[0];
    g = rgbD[1];
    b = rgbD[2];

    h = 0.0;

    if (r > g)
    {
	max = MAX(r, b);
	min = MIN(g, b);
    }
    else
    {
	max = MAX(g, b);
	min = MIN(r, b);
    }
    v = max;

    if (max != 0.0)
	s = (max - min) / max;
    else
	s = 0.0;

    if (s == 0.0)
    {
	h = 0.0;
    }
    else
    {
	delta = max - min;

	if (r == max)
	    h = (g - b) / delta;
	else if (g == max)
	    h = 2 + (b - r) / delta;
	else if (b == max)
	    h = 4 + (r - g) / delta;
	
	h /= 6.0;
	
	if (h < 0.0)
	    h += 1.0;
	else if (h > 1.0)
	    h -= 1.0;
    }

    hsvD[0] = h;
    hsvD[1] = s;
    hsvD[2] = v;

    return 0;
}

extern int hsvD_to_rgbD(double* hsvD,double* rgbD)
{
  double f,p,q,t,h,s,v;

  h = hsvD[0];
  s = hsvD[1];
  v = hsvD[2];

  if (s == 0.0)
    {
      rgbD[0] = v;
      rgbD[1] = v;
      rgbD[2] = v;

      return 1;
    }

  h = h * 6.0;

  if (h == 6.0) h = 0.0;

  f = h - (int)h;
  p = v*(1.0 - s);
  q = v*(1.0 - s*f);
  t = v*(1.0 - s*(1.0 - f));
  
  switch ((int)h)
  {
      case 0:
	  rgbD[0] = v;
	  rgbD[1] = t;
	  rgbD[2] = p;
	  break;
      case 1:
	  rgbD[0] = q;
	  rgbD[1] = v;
	  rgbD[2] = p;
	  break;
      case 2:
	  rgbD[0] = p;
	  rgbD[1] = v;
	  rgbD[2] = t;
	  break;	      
      case 3:
	  rgbD[0] = p;
	  rgbD[1] = q;
	  rgbD[2] = v;
	  break;
      case 4:
	  rgbD[0] = t;
	  rgbD[1] = p;
	  rgbD[2] = v;
	  break;
      case 5:
	  rgbD[0] = v;
	  rgbD[1] = p;
	  rgbD[2] = q;
	  break;
  }

  return 0;
}

/*
  interpolate between two given colours
*/

static int double_interpolate(double z,double a,double b,double *c)
{
  *c = (1.0-z)*a + z*b;

  return 0;
}

static int rgbD_interpolate(double z,double *aD,double *bD,double *cD)
{
  int i,err = 0;

  for (i=0 ; i<3 ; i++)
    err |= double_interpolate(z,aD[i],bD[i],cD+i);

  return err;
}

extern int rgb_interpolate(double z,rgb_t a,rgb_t b,rgb_t *c)
{
  double aD[3], bD[3], cD[3];
  int err = 0;

  err |= rgb_to_rgbD(a,aD);
  err |= rgb_to_rgbD(b,bD);
  err |= rgbD_interpolate(z,aD,bD,cD);
  err |= rgbD_to_rgb(cD,c);

  return err;
}

extern int hsv_interpolate(double z,hsv_t a,hsv_t b,hsv_t *c)
{
  double aD[3], bD[3], cD[3];
  int err = 0;

  err |=  hsv_to_rgbD(a,aD);
  err |=  hsv_to_rgbD(b,bD);
  err |=  rgbD_interpolate(z,aD,bD,cD);
  err |=  rgbD_to_hsv(cD,c);
  
  return err;
}

extern int colour_interpolate(double z,colour_t a,colour_t b,
			      model_t model,colour_t *c)
{
  int err;

  switch (model)
    {
    case rgb: 
      err = rgb_interpolate(z,a.rgb,b.rgb,&(c->rgb));
      break;

    case hsv:
      err = hsv_interpolate(z,a.hsv,b.hsv,&(c->hsv));
      break;

    default:
      return 1;
    }

  return err;
}






