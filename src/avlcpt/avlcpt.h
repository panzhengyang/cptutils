/*
  avlcpt.h

  (c) J.J.Green 2005
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





