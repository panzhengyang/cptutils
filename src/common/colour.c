/*
  colours.c
  
  simple GRB colours for gimpcpt

  (c) J.J.Green 2001
  $Id: colour.c,v 1.4 2001/05/24 01:04:24 jjg Exp $
*/

#define _SVID_SOURCE

#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)>(b)) ? (b) : (a))

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "colour.h"

static double colourD(int);
static int colour8(double);

/*
  convert a triple of [0..1] vals to/from an rgb_t struct
*/

extern int colour_to_rgb(double* col,rgb_t* rgb)
{
    if (!col || !rgb) return 1;

    rgb->red   = colour8(col[0]);
    rgb->green = colour8(col[1]);
    rgb->blue  = colour8(col[2]);

    return 0;
}

extern int rgb_to_colour(rgb_t rgb,double* col)
{
    if (!col) return 1;

    col[0] = colourD(rgb.red);
    col[1] = colourD(rgb.green);
    col[2] = colourD(rgb.blue);

    return 0;
}

/*
  convert a number in [0,1] to/from something in 0...255 
*/

static int colour8(double x)
{
    int i;

    i = (int)(x*256.0 - 0.5);

    i = (i<0 ? 0 : i);
    i = (i>255 ? 255 : i);

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
  taken from libgimp/gimpcolorspace.c
*/
extern int rgb_to_hsv(double r,double g,double b,
		      double* ph,double* ps,double* pv)
{
    double min, max;
    double delta,h,s,v;
    
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

    *ph = h;
    *ps = s;
    *pv = v;

    return 0;
}

extern int hsv_to_rgb(double h,double s,double v,
		      double* r,double* g,double* b)
{
  double f,p,q,t;

  if (s == 0.0)
    {
      *r  = v;
      *g  = v;
      *b  = v;

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
	  *r  = v;
	  *g  = t;
	  *b  = p;
	  break;
      case 1:
	  *r  = q;
	  *g  = v;
	  *b  = p;
	  break;
      case 2:
	  *r  = p;
	  *g  = v;
	  *b  = t;
	  break;	      
      case 3:
	  *r  = p;
	  *g  = q;
	  *b  = v;
	  break;
      case 4:
	  *r  = t;
	  *g  = p;
	  *b  = v;
	  break;
      case 5:
	  *r  = v;
	  *g  = p;
	  *b  = q;
	  break;
  }

  return 0;
}








