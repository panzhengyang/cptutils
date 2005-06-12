/*
  svgcpt.h

  (c) J.J.Green 2004
  $Id: svgcpt.h,v 1.1 2004/09/07 15:53:12 jjg Exp jjg $
*/

#ifndef SVGCPT_H
#define SVGCPT_H

typedef struct
{
  int verbose;
  int debug;
  int n;
  int list;
  struct
  {
    char *input,*output;
  } file;
  struct
  {
    FILE *input,*output;
  } stream;
} svgcpt_opt_t;

extern int svgcpt(svgcpt_opt_t);

#endif



