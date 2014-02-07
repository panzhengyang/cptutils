/*
  pssvg.c
  (c) J.J.Green 2014
*/

#include <stdlib.h>

#include <grd5read.h>
#include <grd5.h>

#include <svgwrite.h>
#include <svg.h>

#include "pssvg.h"

typedef struct
{
  size_t n;
  svg_t **svg;
} svgset_t;

static svgset_t* svgset_new(void)
{
  svgset_t *svgset = malloc(sizeof(svgset_t));
  if (svgset == NULL)
    return NULL;

  svgset->n   = 0;
  svgset->svg = NULL;

  return svgset;
}

static void svgset_destroy(svgset_t *svgset)
{
  int i;

  for (i=0 ; i < svgset->n ; i++)
    svg_destroy(svgset->svg[i]);
  
  free(svgset->svg);
  free(svgset);
}

static int pssvg_convert(grd5_t *grd, svgset_t *svgset, pssvg_opt_t opt)
{
  // FIXME

  return 1;
}

extern int pssvg(pssvg_opt_t opt)
{
  grd5_t *grd5;

  switch (grd5_read(opt.file.input, &grd5))
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

  int err = 0;
  svgset_t *svgset;

  if ((svgset = svgset_new()) == NULL)
    goto cleanup_grd5;

  if (pssvg_convert(grd5, svgset, opt) != 0)
    {
      fprintf(stderr, "conversion failed\n");
      err++;
      goto cleanup_svgset;
    }

  if (svgset->n > 0)
    {
      fprintf(stderr, "no svg gradients converted\n");
      err++;
      goto cleanup_svgset;
    }

  svg_preview_t preview;
  preview.use = false;

  if (svg_write(opt.file.output, 
		svgset->n, 
		(const svg_t**)svgset->svg, 
		&preview) != 0)
    {
      fprintf(stderr, "failed write of svg\n");
      err++;
      goto cleanup_svgset;
    }

 cleanup_svgset:
  svgset_destroy(svgset);

 cleanup_grd5:
  grd5_destroy(grd5);

  return err;
}
