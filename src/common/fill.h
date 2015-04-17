/*
  fill.h
  
  fill types for cptutils

  (c) J.J.Green 2001,2004
*/

#ifndef FILL_H
#define FILL_H

#include "colour.h"

typedef enum {fill_colour,
	      fill_grey,
	      fill_hatch,
	      fill_file,
	      fill_empty} filltype_t;

typedef struct
{
  int sign;
  int n;
  int dpi;
} hatch_t;

typedef struct 
{
  filltype_t type;
  union
  {
    colour_t colour;
    int      grey;
    hatch_t  hatch;
    char*    file;
  } u;
} fill_t;

extern int fill_eq(fill_t, fill_t, model_t);
extern int fill_interpolate(double, fill_t, fill_t, model_t, fill_t*);
extern int fill_rgb(fill_t, model_t, rgb_t*);

#endif

