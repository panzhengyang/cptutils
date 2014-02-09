/*
  grdxsvg.h

  common code for grd3 and grd5 formats, in particular 
  a function taking a gstack of colour stops amd one of
  opacity stops and returning an svg_t struct

  Copyright (c) J.J. Green 2014
*/

#ifndef GRDXSVG_H
#define GRDXSVG_H

#include "gstack.h"
#include "svg.h"

typedef struct 
{
  unsigned int z;
  double r,g,b;
} rgb_stop_t;

typedef struct 
{
  unsigned int z;
  double op;
} op_stop_t;

typedef struct 
{
  unsigned int z;
  double r,g,b,op;
} rgbop_stop_t;

extern int grdxsvg(gstack_t *rgb_stops,
		   gstack_t *op_stops,
		   svg_t *svg);

#endif
