/*
  fill.h
  
  simple GRB fills for gimpcpt

  (c) J.J.Green 2001,2004
  $Id: fill.h,v 1.1 2004/03/18 02:26:45 jjg Exp jjg $
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
extern int fill_interpolate(double,fill_t,fill_t,fill_t*);

#endif

