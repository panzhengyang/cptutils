/*
  svgcpt.h

  (c) J.J.Green 2004
  $Id: svgcpt.h,v 1.2 2004/04/12 15:22:18 jjg Exp $
*/

#ifndef SVGCPT_H
#define SVGCPT_H

typedef struct
{
  int verbose;
  int debug;
  struct
  {
    char *input,*output;
  } file;
} svgcpt_opt_t;

extern int svgcpt(svgcpt_opt_t);

#endif



