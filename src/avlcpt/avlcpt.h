/*
  avlcpt.h

  (c) J.J.Green 2005
  $Id: pspcpt.h,v 1.1 2005/01/27 21:31:30 jjg Exp $
*/

#ifndef AVLCPT_H
#define AVLCPT_H

typedef struct
{
  int verbose;
  int debug;
  double precision;
  const char* label;
  struct
  {
    char *input,*output;
  } file;
} avlcpt_opt_t;

extern int avlcpt(avlcpt_opt_t);

#endif





