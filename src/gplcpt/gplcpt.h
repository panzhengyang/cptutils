/*
  gplcpt.h
  convert GIMP palette (gpl) to GMT colour table (cpt)
  Copyright (c) J.J. Green 2010
*/

#ifndef GPLCPT_H
#define GPLCPT_H

#include "colour.h"

typedef struct
{
  int verbose;
  rgb_t fg,bg,nan;
  struct
  {
    char *input,*output;
  } file;
} gplcpt_opt_t;

extern int gplcpt(gplcpt_opt_t);

#endif
