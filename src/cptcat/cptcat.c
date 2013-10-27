/*
  cptcat.c

  Copyright (c) J.J. Green 2013
*/

#include <stdio.h>
#include <stdlib.h>

#include <cpt.h>
#include <cptio.h>

#include "cptcat.h"

typedef struct
{
  cpt_t *cpt;
  const char *file;
  double range[2];
} cptdat_t;

static int datcmp(const cptdat_t *a, const cptdat_t *b)
{
  if (a->range[0] < b->range[0])
    return -1;
  if (a->range[0] > b->range[0])
    return 1;
  return 0;
}

extern int cptcat(cptcat_opt_t opt)
{
  int i, n = opt.input.n;
  cptdat_t dat[n];

  /* load cpts */

  for (i=0 ; i<n ; i++)
    {
      dat[i].file = opt.input.file[i];

      if ((dat[i].cpt = cpt_new()) == NULL)
	return 1;

      if (cpt_read(dat[i].file, dat[i].cpt, 0) != 0)
	{
	  fprintf(stderr, "failed to read %s\n", dat[i].file);
	  return 1;
	}

      if (cpt_zrange(dat[i].cpt, dat[i].range) != 0)
	{
	  fprintf(stderr, "failed to get zrange\n");
	  return 1;
	}

      if (opt.verbose)
	printf("  %s\n", dat[i].file);
    }

  /*
    check increasing -- we could coerce, but that can
    can be done by applying makecpt to the offending
    component file.
  */

  for (i=0 ; i<n ; i++)
    {
      double *range = dat[i].range;

      if (range[0] >= range[1])
	{
	  fprintf(stderr, "%s is decreasing\n", dat[i].file); 
	  return 1;
	}
    }

  /* sort */

  qsort(dat, n, sizeof(cptdat_t),
	(int (*)(const void *, const void *))datcmp);

  /* check contiguous */

  for (i=0 ; i<n-1 ; i++)
    {
      if (dat[i].range[1] != dat[i+1].range[0])
	{
	  fprintf(stderr, "non-contiguous input:\n");
	  fprintf(stderr, "  %s : %g < z < %g\n", 
		  dat[i].file, dat[i].range[0], dat[i].range[1]);
	  fprintf(stderr, "  %s : %g < z < %g\n", 
		  dat[i+1].file, dat[i+1].range[0], dat[i+1].range[1]);
	  return 1;
	}
    }

  /* check all have the same model */

  model_t model = dat[0].cpt->model;

  for (i=1 ; i<n ; i++)
    {
      if (dat[0].cpt->model != model)
	{
	  fprintf(stderr, "incompatible colour models in input\n");
	  return 1;
	}
    }

  /* create the new cpt */

  cpt_t *cpt;

  if ((cpt = cpt_new()) == NULL)
    return 1;

  cpt->model = model;

  cpt->nan = dat[0].cpt->nan;
  cpt->bg  = dat[0].cpt->bg;
  cpt->fg  = dat[n-1].cpt->fg;

  for (i=0 ; i<n ; i++)
    {
      cpt_seg_t *seg;

      while ((seg = cpt_pop(dat[i].cpt)) != NULL)
	{
	  if (cpt_append(seg, cpt) != 0)
	    {
	      fprintf(stderr, "failed prepend\n");
	      return 1;
	    }
	}
    }

  /* create hyphenated name for new file */

  char *name = cpt->name;
  int nc = 0;
  for (i=0 ; i<n-1 ; i++)
    nc += snprintf(name+nc, CPT_NAME_LEN-nc, "%s-", dat[i].cpt->name);
  nc += snprintf(name+nc, CPT_NAME_LEN-nc, "%s", dat[n-1].cpt->name);

  if (nc >= CPT_NAME_LEN)
    {
      fprintf(stderr, "output name truncated\n");
      name[CPT_NAME_LEN-1] = '\0';
    }

  /* destroy components */

  for (i=0 ; i<n ; i++)
    cpt_destroy(dat[i].cpt);

  /* write to file*/

  if (cpt_write(opt.output, cpt) != 0)
    {
      fprintf(stderr, "failed write to %s\n", opt.output);
      return 1;
    }

  return 0;
}