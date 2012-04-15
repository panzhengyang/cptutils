/*
  pspsvg.h

  (c) J.J.Green 2011
  $Id: pspsvg.h,v 1.1 2012/01/22 20:08:51 jjg Exp jjg $
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
  sgv_preview_t preview;
} pspsvg_opt_t;

extern int pspsvg(pspsvg_opt_t);

#endif





