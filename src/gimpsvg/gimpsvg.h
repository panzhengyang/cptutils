/*
  gimpsvg.h

  (c) J.J.Green 2011
*/

#ifndef GIMPSVG_H
#define GIMPSVG_H

#include <stdbool.h>
#include <stdlib.h>

#include "colour.h"
#include "svgpreview.h"

typedef struct 
{
  bool verbose, reverse;
  size_t samples;
  double tol;
  svg_preview_t preview;
} gimpsvg_opt_t;

extern int gimpsvg(const char*, const char*, gimpsvg_opt_t);

#endif





