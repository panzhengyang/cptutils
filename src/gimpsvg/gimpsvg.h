/*
  gimpsvg.h

  (c) J.J.Green 2011
  $Id: gimpsvg.h,v 1.3 2011/11/10 23:34:27 jjg Exp jjg $
*/

#ifndef GIMPSVG_H
#define GIMPSVG_H

#include <stdbool.h>
#include <stdlib.h>

#include "colour.h"

typedef struct 
{
  bool verbose, reverse;
  rgb_t fg, bg, nan;
  size_t samples;
  double min, max;
  double tol;
} gimpsvg_opt_t;

extern int gimpsvg(const char*, const char*, gimpsvg_opt_t);

#endif





