/*
  svgpreview.h

  specify a preview image in SVG output

  Copyright (c) J.J. Green 2012
  $Id: svgpreview.h,v 1.1 2012/04/15 17:50:10 jjg Exp jjg $
*/

#ifndef SVGPREVIEW_H
#define SVGPREVIEW_H

#include <stdbool.h>

typedef struct
{
  /* whether or not to include a preview */
  bool use;
  /* the geometry of the preview in pixels */
  size_t width, height, border, stroke;
} svg_preview_t;

extern int svg_preview_geometry(const char*, svg_preview_t*);

#endif
