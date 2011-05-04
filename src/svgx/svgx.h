/*
  svgx.h

  (c) J.J.Green 2004
  $Id: svgx.h,v 1.7 2010/11/01 18:47:13 jjg Exp jjg $
*/

#ifndef SVGX_H
#define SVGX_H

#include "colour.h"

typedef enum {type_cpt, type_ggr, type_pov, type_gpt, type_css3, type_psp} 
  svgx_type_t;

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



