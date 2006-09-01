/*
  svgx.h

  (c) J.J.Green 2004
  $Id: svgx.h,v 1.5 2005/12/04 19:39:22 jjg Exp jjg $
*/

#ifndef SVGX_H
#define SVGX_H

#include "colour.h"

typedef enum {type_cpt, type_ggr, type_pov, type_psp} svgx_type_t;

typedef struct
{
  int verbose;
  int permissive;
  int debug;
  int all;
  int first;
  int list;
  char *name;
  rgb_t fg,bg,nan;
  svgx_type_t type; 
  struct
  {
    char *file;
    FILE *stream;;
  } input,output;
} svgx_opt_t;

extern int svgx(svgx_opt_t);

#endif



