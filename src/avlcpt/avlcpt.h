/*
  avlcpt.h

  (c) J.J.Green 2005
  $Id: avlcpt.h,v 1.1 2005/11/19 16:54:08 jjg Exp jjg $
*/

#ifndef AVLCPT_H
#define AVLCPT_H

typedef struct
{
  int verbose;
  int debug;
  double precision;
  struct
  {
    int upper,lower;
  } adjust;
  int reverse;
  struct
  {
    char *input,*output;
  } file;
} avlcpt_opt_t;

extern int avlcpt(avlcpt_opt_t);

#endif





