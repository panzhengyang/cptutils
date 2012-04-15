/*
  svgpreview.h

  specify a preview image in SVG output

  Copyright (c) J.J. Green 2012
  $Id$
*/

#ifndef SVGPREVIEW_H
#define SVGPREVIEW_H

#include <stdbool.h>

typedef struct
{
  bool use;
  double width, height;
} svg_preview_t;

#endif
