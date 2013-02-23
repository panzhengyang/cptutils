/*
  cptsvg.h

  J.J.Green 2004
*/

#ifndef CPTSVG_H
#define CPTSVG_H

#include "svgpreview.h"

typedef struct cptsvg_opt_t
{
  int verbose;
  svg_preview_t preview;
} cptsvg_opt_t;

extern int cptsvg(char*, char*, cptsvg_opt_t);

#endif
