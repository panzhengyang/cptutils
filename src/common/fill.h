/*
  fill.h
  
  fill types for cptutils

  (c) J.J.Green 2001,2004
  $Id: fill.h,v 1.2 2004/04/12 23:42:14 jjg Exp jjg $
*/

#ifndef FILL_H
#define FILL_H

#include "colour.h"

typedef enum {colour,grey,hatch,file,empty} filltype_t;

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

extern int fill_eq(fill_t,fill_t);
extern int fill_interpolate(double,fill_t,fill_t,model_t,fill_t*);
extern int fill_rgb(fill_t,model_t,rgb_t*);

#endif

