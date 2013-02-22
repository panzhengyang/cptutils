/*
  cptsvg.h

  J.J.Green 2004
  $Id: cptsvg.h,v 1.2 2012/04/15 17:53:20 jjg Exp $
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
