/*
  svgcpt.h

  (c) J.J.Green 2004
  $Id: svgcpt.h,v 1.2 2005/06/12 23:18:20 jjg Exp jjg $
*/

#ifndef SVGCPT_H
#define SVGCPT_H

typedef struct
{
  int verbose;
  int debug;
  int all;
  int list;
  char *name;
  struct
  {
    char *file;
    FILE *stream;;
  } input,output;
} svgcpt_opt_t;

extern int svgcpt(svgcpt_opt_t);

#endif



