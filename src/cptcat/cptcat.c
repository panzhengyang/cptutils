/*
  cptcat.c

  Copyright (c) J.J. Green 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cptwrite.h>
#include <cptread.h>
#include <btrace.h>

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

      if (cpt_read(dat[i].file, dat[i].cpt) != 0)
	{
	  btrace("failed to read %s", dat[i].file);
	  return 1;
	}

      if (cpt_zrange(dat[i].cpt, dat[i].range) != 0)
	{
	  btrace("failed to get zrange");
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
	  btrace("%s is decreasing", dat[i].file); 
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
	  btrace("non-contiguous input:");
	  btrace("  %s : %g < z < %g", 
		     dat[i].file, dat[i].range[0], dat[i].range[1]);
	  btrace("  %s : %g < z < %g", 
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
	  btrace("incompatible colour models in input");
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
	      btrace("failed prepend");
	      return 1;
	    }
	}
    }

  /*
    create hyphenated name for new file, this has length
    which is the sum of lengths of the substrings, plus
    n-1 hyphens, plus a termating null.
  */

  size_t nctot = 0;

  for (i=0 ; i<n ; i++)
    nctot += (dat[i].cpt->name ? strlen(dat[i].cpt->name) : 1);

  cpt->name = malloc(nctot + n);

  strcpy(cpt->name, (dat[0].cpt->name ? dat[0].cpt->name : "x"));
  for (i=1 ; i<n ; i++)
    {
      strcat(cpt->name, "-");
      strcat(cpt->name, (dat[i].cpt->name ? dat[i].cpt->name : "x"));
    }

  /* destroy components */

  for (i=0 ; i<n ; i++)
    cpt_destroy(dat[i].cpt);

  /* write to file*/

  if (cpt_write(opt.output, cpt) != 0)
    {
      btrace("failed write to %s", opt.output);
      return 1;
    }

  return 0;
}
