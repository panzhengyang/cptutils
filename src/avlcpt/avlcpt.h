/*
  avlcpt.h

  (c) J.J.Green 2005
  $Id: avlcpt.h,v 1.4 2006/07/28 22:58:37 jjg Exp $
*/

#ifndef AVLCPT_H
#define AVLCPT_H

#include "colour.h"

typedef struct
{
  int verbose;
  int debug;
  double precision;
  rgb_t fg,bg,nan;
  struct
  {
    int upper,lower;
  } adjust;
  struct
  {
    char *input,*output;
  } file;
} avlcpt_opt_t;

extern int avlcpt(avlcpt_opt_t);

#endif





