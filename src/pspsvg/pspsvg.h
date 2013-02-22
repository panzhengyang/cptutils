/*
  pspsvg.h

  (c) J.J.Green 2011
  $Id: pspsvg.h,v 1.3 2012/04/16 21:14:40 jjg Exp $
*/

#ifndef PSPSVG_H
#define PSPSVG_H

#include "svgpreview.h"

typedef struct
{
  int verbose;
  int debug;
  struct
  {
    char *input,*output;
  } file;
  svg_preview_t preview;
} pspsvg_opt_t;

extern int pspsvg(pspsvg_opt_t);

#endif





