/*
  svgx.h

  (c) J.J.Green 2004
*/

#ifndef SVGX_H
#define SVGX_H

#include <stdbool.h>

#include "svgpreview.h"
#include "colour.h"

typedef enum {
  type_cpt, 
  type_ggr, 
  type_pov, 
  type_gpt, 
  type_css3, 
  type_grd3,
  type_sao, 
  type_png, 
  type_svg
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
  char *name;
  struct
  {
    rgb_t alpha;
    struct
    {
      rgb_t fg,bg,nan;
    } cpt;
    struct
    {
      size_t width,height;
    } png;
    struct
    {
      svg_preview_t preview;
    } svg;
  } format;
  struct
  {
    char *file;
    FILE *stream;;
  } input, output;
} svgx_opt_t;

extern int svgx(svgx_opt_t);

#endif



