/*
  colours.c
  
  colours for gimpcpt

  (c) J.J.Green 2001,2004
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

extern double colour_rgb_dist(colour_t a, colour_t b, model_t model)
{
  double da[3] = {0}, db[3] = {0}, sum;
  int i;

  switch (model)
    {
    case model_rgb :
      rgb_to_rgbD(a.rgb, da); 
      rgb_to_rgbD(b.rgb, db); 
      break;
      
    case model_hsv :
      hsv_to_rgbD(a.hsv, da); 
      hsv_to_rgbD(b.hsv, db); 
      break;

    default:

      return -1.0;
    }

  for (sum=0.0, i=0 ; i<3 ; i++)
    {
      double d = da[i] - db[i];
      sum += d*d;
    }

  return sqrt(sum);
}

/*
  convert a gimp triple of [0..1] vals to/from an rgb_t struct
*/

extern int rgbD_to_rgb(const double col[static 3], rgb_t* rgb)
{
  rgb->red   = colour8(col[0]);
  rgb->green = colour8(col[1]);
  rgb->blue  = colour8(col[2]);
  
  return 0;
}

extern int rgb_to_rgbD(rgb_t rgb, double col[static 3])
{
  col[0] = colourD(rgb.red);
  col[1] = colourD(rgb.green);
  col[2] = colourD(rgb.blue);
  
  return 0;
}

extern int grey_to_rgbD(int g, double col[static 3])
{
  rgb_t rgb;

  rgb.red   = g;
  rgb.green = g;
  rgb.blue  = g;

  return rgb_to_rgbD(rgb, col);
}

/*
  convert gimp rgbD to/from gmt hsv type -- this is
  needed since gmt stores its hsv format as hsv triples,
  whereas gimp converts them to rgb, reflecting the format's
  purpose  : the former is easier for hand-crafting, the
  latter for automated processing of GUI generation.
*/

extern int rgbD_to_hsv(const double rgbD[static 3], hsv_t* hsv)
{
  double hsvD[3] = {0};
  
  if (rgbD_to_hsvD(rgbD, hsvD) != 0)
    return 1;
  
  hsv->hue = hsvD[0] * 360.0;
  hsv->sat = hsvD[1];
  hsv->val = hsvD[2];

  return 0;
}

extern int hsv_to_rgbD(hsv_t hsv, double rgbD[static 3])
{
  double hsvD[3] = {0};
  
  hsvD[0] = hsv.hue/360.0;
  hsvD[1] = hsv.sat;
  hsvD[2] = hsv.val;

  if (hsvD_to_rgbD(hsvD, rgbD) != 0)
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
  
  i = MAX(i, 0);
  i = MIN(i, 255);

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

extern int parse_rgb(const char *string, rgb_t *col)
{
  char *dup, *token;
  
  if (string == NULL) return 1;
  if ((dup = strdup(string)) == NULL) return 1;
  
  token = strtok(dup, "/");
  
  col->red = atoi(token);
  
  if ((token = strtok(NULL, "/")) == NULL)
    {
      col->blue = col->green = col->red;
      free(dup);
      return 0;
    }
  
  col->green = atoi(token);
  
  if ((token = strtok(NULL, "/")) == NULL)
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

extern int rgbD_to_hsvD(const double rgbD[static 3], double hsvD[static 3])
{
  double min, max;
  double r, g, b, h, s, v;
  
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
      double delta = max - min;
      
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

extern int hsvD_to_rgbD(const double hsvD[static 3], double rgbD[static 3])
{
  double f, p, q, t, h, s, v;

  h = hsvD[0];
  s = hsvD[1];
  v = hsvD[2];

  if (s == 0.0)
    {
      rgbD[0] = v;
      rgbD[1] = v;
      rgbD[2] = v;

      return 0;
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
    default:
      return 1;
    }
  
  return 0;
}

/*
  interpolate between two given colours
*/

static int double_interpolate(double z, double a, double b, double *c)
{
  *c = (1.0-z)*a + z*b;

  return 0;
}

static int rgbD_interpolate(double z, 
			    const double aD[static 3], 
			    const double bD[static 3], 
			    double cD[static 3])
{
  int i,err = 0;

  for (i=0 ; i<3 ; i++)
    err |= double_interpolate(z, aD[i], bD[i], cD+i);

  return err;
}

extern int rgb_interpolate(double z, rgb_t a, rgb_t b, rgb_t *c)
{
  double aD[3] = {0}, bD[3] = {0}, cD[3] = {0};
  int err = 0;

  err |= rgb_to_rgbD(a, aD);
  err |= rgb_to_rgbD(b, bD);
  err |= rgbD_interpolate(z, aD, bD, cD);
  err |= rgbD_to_rgb(cD, c);

  return err;
}

extern int hsv_interpolate(double z, hsv_t a, hsv_t b, hsv_t *c)
{
  double 
    aD[3] = {0}, bD[3] = {0}, cD[3] = {0};
  int err = 0;

  err |=  hsv_to_rgbD(a, aD);
  err |=  hsv_to_rgbD(b, bD);
  err |=  rgbD_interpolate(z, aD, bD, cD);
  err |=  rgbD_to_hsv(cD, c);
  
  return err;
}

extern int colour_interpolate(double z, colour_t a, colour_t b,
			      model_t model, colour_t *c)
{
  int err;

  switch (model)
    {
    case model_rgb: 
      err = rgb_interpolate(z, a.rgb, b.rgb, &(c->rgb));
      break;

    case model_hsv:
      err = hsv_interpolate(z, a.hsv, b.hsv, &(c->hsv));
      break;

    default:
      return 1;
    }

  return err;
}






