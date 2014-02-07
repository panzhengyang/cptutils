/*
  svgwrite.h

  writes svg gradient files

  J.J. Green 2005
*/

#ifndef SVGWRITE_H
#define SVGWRITE_H

#include "svg.h"
#include "svgpreview.h"

extern int svg_write(const char*, size_t, const svg_t**, const svg_preview_t*);

#endif
