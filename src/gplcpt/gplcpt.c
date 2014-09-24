/*
  gplcpt.h
  convert GIMP palette (gpl) to GMT colour table (cpt)
  Copyright (c) J.J. Green 2010
*/

#include <stdio.h>
#include <string.h>

#include <cpt.h>
#include <cptwrite.h>
#include <btrace.h>

#include "gplcpt.h"

static int gplcpt_st(gplcpt_opt_t, FILE*);

extern int gplcpt(gplcpt_opt_t opt)
{
  int err = 0;

  if (opt.file.input)
    {
      FILE* st;

      if ((st = fopen(opt.file.input, "r")) == NULL)
        {
          btrace_add("failed to open %s", opt.file.input);
          err++;
        }
      else
	{
	  err = gplcpt_st(opt, st);
	  fclose(st);
	}
    }
  else
    err = gplcpt_st(opt, stdin);

  return err;
}

#define BUFSIZE 1024

static int gplcpt_write(gplcpt_opt_t, cpt_t*);
static int skipline(const char*);

extern int gplcpt_st(gplcpt_opt_t opt, FILE *st)
{
  char buf[BUFSIZE];

  /* get first line */

  if (fgets(buf, BUFSIZE, st) == NULL) 
    {
      btrace_add("no first data line");
      return 1;
    }

  /* check it is a gimp palette */

  if (strncmp(buf, "GIMP Palette", 12) != 0)
    {
      btrace_add("file does not seem to be a GIMP palette");
      btrace_add("%s", buf);
      return 1;
    }

  /* create cpt struct */

  cpt_t *cpt;

  if ((cpt = cpt_new()) == NULL) return 1;

  cpt->model = model_rgb;

  /* set bg/fg/nan values */
  
  cpt->fg.type = cpt->bg.type = cpt->nan.type = fill_colour;

  cpt->bg.u.colour.rgb  = opt.bg;
  cpt->fg.u.colour.rgb  = opt.fg;
  cpt->nan.u.colour.rgb = opt.nan;

  /* get next non-comment line */

  do
    if (fgets(buf, BUFSIZE, st) == NULL) return 1;
  while (skipline(buf));

  /* see if it is a name line */
  
  char name[BUFSIZE];

  if (sscanf(buf, "Name: %s", name) == 1)
    {
      if (opt.verbose)
	{
	  printf("GIMP palette %s\n", name);
	}

      cpt->name = strdup(name);

      /* skip to next non-comment */

      do
	if (fgets(buf, BUFSIZE, st) == NULL) return 1;
      while (skipline(buf));
    }

  /* see if it is columns line */

  int columns;

  if (sscanf(buf, "Columns: %i", &columns) == 1)
    {
      if (opt.verbose)
	{
	  printf("%i columns\n", columns);
	}

      /* skip to next non-comment */

      do
	if (fgets(buf, BUFSIZE, st) == NULL) return 1;
      while (skipline(buf));
    }

  /* now at the rgb data we hope */

  fill_t fill;
  fill.type = fill_colour;

  size_t n = 0;

  while (1)
    {
      int r, g, b;

      if (sscanf(buf, "%i %i %i", &r, &g, &b) != 3)
	{
	  btrace_add("bad line %s", buf);
	  return 1;
	} 

      fill.u.colour.rgb.red   = r;
      fill.u.colour.rgb.green = g;
      fill.u.colour.rgb.blue  = b;

      cpt_seg_t* seg;

      if ((seg = cpt_seg_new()) == NULL)
	{
	  btrace_add("failed to create new segment");
	  return 1;
	}

      seg->lsmp.val  = n;
      seg->lsmp.fill = fill;

      seg->rsmp.val  = n+1;
      seg->rsmp.fill = fill;

      if (cpt_append(seg, cpt) != 0)
	{
	  btrace_add("failed append of segment %i", (int)n);
	  return 1;
	}

      n++;

      do
	if (fgets(buf, BUFSIZE, st) == NULL)
	  {
	    if (opt.verbose)
	      {
		printf("read %zu RGB triples\n", n);
	      }

	    return gplcpt_write(opt, cpt);
	  }
      while (skipline(buf));
    }

  return 1;
}

static int cpt_step_optimise(cpt_t*);

static int gplcpt_write(gplcpt_opt_t opt, cpt_t *cpt)
{
  if (cpt_step_optimise(cpt) != 0)
    {
      btrace_add("failed optimise");
      return 1;
    }

  if (opt.verbose)
    {
      printf("converted to %i segment cpt\n", cpt_nseg(cpt));
    }

  return cpt_write(opt.file.output, cpt);
}

/*
  aggregates segments which have the same fill, 
  assuming that the input consists of piecewise
  constant segments (as here) 
*/

static int cpt_step_optimise(cpt_t* cpt)
{
  cpt_seg_t *left, *right;

  left  = cpt->segment;
  right = left->rseg; 

  while (right)
    {
      if (fill_eq(left->rsmp.fill, right->lsmp.fill))
	{
	  cpt_seg_t* dead = right;

	  if ((right = dead->rseg) != NULL)
	    {
	      right->lseg = left;
	    }

	  left->rsmp.val = dead->rsmp.val;
	  left->rseg = right;
	  
	  cpt_seg_destroy(dead);
	}
      else
	{
	  left  = left->rseg;
	  right = right->rseg;
	}
    }

  return 0;
}

/* 
   returns whether the line is a comment line or not, 
   it must not be null
*/

static int skipline(const char* line)
{
  const char *s;

  if (line == NULL) return 1;
  
  s = line; 

  do {
    switch (*s)
      {
      case '#':
      case '\n':
      case '\0':
        return 1;
      case ' ':
      case '\t':
        break;
      default:
        return 0;
      }
  }
  while (s++);

  return 0;
}

