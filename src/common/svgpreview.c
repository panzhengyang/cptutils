/*
  svgpreview.h

  specify a preview image in SVG output

  Copyright (c) J.J. Green 2012
  $Id: svgpreview.h,v 1.1 2012/04/15 17:50:10 jjg Exp jjg $
*/

#include <stdio.h>
#include "svgpreview.h"

typedef struct 
{
  char c;
  const char name[32];
  double ppt;
} unit_t;

#define NUM_UNITS 4

#define PPT_PER_PT 0.99626401 
#define PPT_PER_IN 72.0
#define PPT_PER_MM 2.83464567
#define PPT_PER_CM (10.0*PPT_PER_MM)

static unit_t u[NUM_UNITS] = 
  {
    {'p',"Postscript point",1.0},
    {'i',"inch",PPT_PER_IN},
    {'m',"millimeter",PPT_PER_MM},
    {'c',"centimeter",PPT_PER_CM}
  };

static double unit_ppt(char c)
{
  int i;

  for (i=0 ; i<NUM_UNITS ; i++)
    if (u[i].c == c) return u[i].ppt; 

  return -1.0;
}

static int unit_list_stream(FILE* st)
{
  int i;

  for (i=0 ; i<NUM_UNITS ; i++)
    fprintf(st,"%c - %s\n",u[i].c,u[i].name); 

  return 0;
}

static int scan_length(const char *p, const char *name, double *x)
{ 
  char c;
  double u;
  
  switch (sscanf(p,"%lf%c",x,&c))  
    {
    case 0: 
      fprintf(stderr,"%s option missing an argument\n",name);
      return 1;
    case 1: c = 'p';
    case 2: 
      if ((u = unit_ppt(c)) <= 0)
        {
          fprintf(stderr,"unknown unit %c in %s %s\n",c,name,p);
          unit_list_stream(stderr);
          return 1;
        }
      break;
    default:
      return 1;
    }

  *x *= u;

  return 0;
}

/*
  scan a string of the form <double>[<unit>][/<double>[<unit>]]
  and fill in the geometry section of the svg preview struct
*/

extern int svg_preview_geometry(const char *str, svg_preview_t *preview)
{
  preview->use = true;
  preview->width = 200;
  preview->height = 50;

  return 0;
}
