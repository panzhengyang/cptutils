/*
  pspsvg.h

  (c) J.J.Green 2011
  $Id: pspsvg.h,v 1.2 2005/12/04 18:49:57 jjg Exp $
*/

#ifndef PSPSVG_H
#define PSPSVG_H

typedef struct
{
  int verbose;
  int debug;
  struct
  {
    char *input,*output;
  } file;
} pspsvg_opt_t;

extern int pspsvg(pspsvg_opt_t);

#endif





