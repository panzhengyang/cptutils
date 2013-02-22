/*
  xycpt.h

  (c) J.J.Green 2004
  $Id: xycpt.h,v 1.3 2005/12/03 00:17:35 jjg Exp $
*/

#ifndef XYCPT_H
#define XYCPT_H

#include "colour.h"

typedef enum {reg_lower,reg_middle,reg_upper} reg_t;

typedef struct
{
  int verbose;
  int debug;
  int discrete;
  reg_t reg;
  int unital;
  rgb_t fg,bg,nan;
  struct
  {
    char *input,*output;
  } file;
} xycpt_opt_t;

extern int xycpt(xycpt_opt_t);

#endif





