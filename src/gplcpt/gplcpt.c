/*
  gplcpt.h
  convert GIMP palette (gpl) to GMT colour table (cpt)
  Copyright (c) J.J. Green 2010
  $Id$
*/

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <math.h>

#include "gplcpt.h"

static int gplcpt_st(gplcpt_opt_t,FILE*);

extern int gplcpt(gplcpt_opt_t opt)
{
  int err = 0;

  if (opt.file.input)
    {
      FILE* st;

      if ((st = fopen(opt.file.input,"r")) == NULL)
        {
          fprintf(stderr,"failed to open %s\n",opt.file.input);
          err++;
        }
      else
	{
	  err = gplcpt_st(opt,st);
	  fclose(st);
	}
    }
  else
    err = gplcpt_st(opt,stdin);

  return err;
}

#define BUFSIZE 1024

static int skipline(const char*);

extern int gplcpt_st(gplcpt_opt_t opt,FILE *st)
{
  char buf[BUFSIZE];

  /* get first line */

  if (fgets(buf,BUFSIZE,st) == NULL) 
    {
      fprintf(stderr,"no first data line\n");
      return 1;
    }

  /* check it is a gimp palette */

  if (strncmp(buf,"GIMP Palette",12) != 0)
    {
      fprintf(stderr,"file does not seem to be a GIMP palette\n");
      fprintf(stderr,"%s",buf);
      return 1;
    }

  /* get next non-comment line */

  do
    if (fgets(buf,BUFSIZE,st) == NULL) return 1;
  while (skipline(buf));

  /* see if it is a name line */
  
  char name[BUFSIZE];

  if (sscanf(buf,"Name: %s",name) == 1)
    {
      if (opt.verbose)
	{
	  printf("GIMP palette %s\n",name);
	}

      /* skip to next non-comment */

      do
	if (fgets(buf,BUFSIZE,st) == NULL) return 1;
      while (skipline(buf));
    }

  /* see if it is columns line */

  int columns;

  if (sscanf(buf,"Columns: %i",&columns) == 1)
    {
      if (opt.verbose)
	{
	  printf("%i columns\n",columns);
	}

      /* skip to next non-comment */

      do
	if (fgets(buf,BUFSIZE,st) == NULL) return 1;
      while (skipline(buf));
    }

  /* now at the rgb data we hope */

  while (1)
    {
      int r,g,b;

      if (sscanf(buf,"%i %i %i",&r,&g,&b) != 3)
	{
	  fprintf(stderr,"bad line %s\n",buf);
	  return 1;
	} 

      /* FIXME */

      printf("%3i %3i %3i\n",r,g,b);

      do
	if (fgets(buf,BUFSIZE,st) == NULL)
	  {
	    /* end of data ... */

	    printf("reading finished\n");

	    return 0;
	  }
      while (skipline(buf));
    }

  return 1;
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

