/*
  svgpreview.h

  specify a preview image in SVG output

  Copyright (c) J.J. Green 2012
  $Id: svgpreview.c,v 1.2 2012/04/15 20:50:10 jjg Exp jjg $
*/

#include <stdio.h>
#include <string.h>

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
  and fill in the geometry section of the svg preview struct.
  the preview->use should be true, else the string will not 
  be inspected (it might be NULL).
*/

extern int svg_preview_geometry(const char *geom, svg_preview_t *preview)
{
  if (! preview->use) return 0;
  
  /* create non-const copy of geom */

  size_t n = strlen(geom); 
  char s[n+1]; 
  
  if (strncpy(s,geom,n+1) == NULL) 
    return 1;

  /* find seperator */

  char *sep = strchr(s,'x');

  if (sep)
    {
      /* split the string and scan each part */

      *sep = '\0';
      
      if (scan_length(s,"width",&(preview->width)) != 0)
	return 1;

      if (scan_length(sep+1,"height",&(preview->height)) != 0)
	return 1;
    }
  else
    {
      /* scan the whole string */

      if (scan_length(s,"width",&(preview->width)) != 0)
	return 1;

      preview->height = preview->width;
    }

  return 0;
}
