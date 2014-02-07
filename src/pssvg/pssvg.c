/*
  pssvg.c

  (c) J.J.Green 2014
*/

#include <stdlib.h>

#include <grd5read.h>
#include <grd5.h>

#include "pssvg.h"

extern int pssvg(pssvg_opt_t opt)
{
  int err;
  grd5_t *grd5;

  switch (err = grd5_read(opt.file.input, &grd5))
    {
    case GRD5_READ_OK: 
      break;
    case GRD5_READ_FOPEN:
      fprintf(stderr, "failed to read %s\n", 
	      (opt.file.input ? opt.file.input : "stdin"));
      return 1;
    case GRD5_READ_FREAD:
      fprintf(stderr, "failed read from stream\n");
      return 1;
    case GRD5_READ_PARSE:
      fprintf(stderr, "failed to parse input\n");
      return 1;
    case GRD5_READ_MALLOC:
      fprintf(stderr, "out of memory\n");
      return 1;
    case GRD5_READ_BUG:
      /* fall-through */
    default:
      fprintf(stderr, "internal error - please report this\n");
      return 1;
    }

  grd5_destroy(grd5);

  return 0;
}
