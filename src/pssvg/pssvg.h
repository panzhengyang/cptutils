/*
  pssvg.h

  (c) J.J.Green 2004
*/

#ifndef PSSVG_H
#define PSSVG_H

#include <stdbool.h>
#include <stdio.h>

#include "svgpreview.h"

typedef enum {
  job_first,
  job_list,
  job_named,
  job_all
} pssvg_job_t;

typedef struct
{
  pssvg_job_t job;
  bool verbose, debug;
  char *name;
  svg_preview_t preview;
  struct
  {
    char *file;
    FILE *stream;;
  } input, output;
} pssvg_opt_t;

extern int pssvg(pssvg_opt_t);

#endif



