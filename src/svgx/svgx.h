/*
  svgx.h

  (c) J.J.Green 2004
  $Id: svgx.h,v 1.11 2011/11/13 12:12:52 jjg Exp jjg $
*/

#ifndef SVGX_H
#define SVGX_H

#include <stdbool.h>

#include "colour.h"

typedef enum {
  type_cpt, 
  type_ggr, 
  type_pov, 
  type_gpt, 
  type_css3, 
  type_psp,
  type_sao, 
  type_png 
} svgx_type_t;

typedef enum {
  job_first,
  job_list,
  job_named,
  job_all
} svgx_job_t;

typedef struct
{
  svgx_type_t type; 
  svgx_job_t job;
  bool verbose, permissive, debug;
  size_t width,height;
  char *name;
  rgb_t fg,bg,nan;
  rgb_t alpha;
  struct
  {
    char *file;
    FILE *stream;;
  } input, output;
} svgx_opt_t;

extern int svgx(svgx_opt_t);

#endif



