/*
  pssvg.c

  (c) J.J.Green 2014
*/

#include "pssvg.h"

#include <grd5.h>
#include <grd5read.h>

extern int pssvg(pssvg_opt_t opt)
{
  grd5_t grd5;
  int err;

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
    case GRD5_READ_BUFFER:
      fprintf(stderr, "buffer overflow\n");
      return 1;
    case GRD5_READ_BUG:
      /* fall-through */
    default:
      fprintf(stderr, "internal error - please report this\n");
      return 1;
    }

  return 0;
}
