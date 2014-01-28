/*
  grd5read.c
  Copyright (c) J.J. Green 2014
*/

#include "grd5read.h"

#include <stdio.h>

static int grd5_read_stream(FILE* stream, grd5_t* grd5);

extern int grd5_read(const char* file, grd5_t* grd5)
{
  int err = GRD5_READ_BUG;

  if (file)
    {
      FILE *stream;
      
      if ((stream = fopen(file, "r")) == NULL)
	return GRD5_READ_FOPEN;

      err = grd5_read_stream(stream, grd5);

      fclose(stream);
    }
  else
    err = grd5_read_stream(stdin, grd5);

  return err;
}

static int grd5_read_stream(FILE* stream, grd5_t* grd5)
{
  return GRD5_READ_PARSE;
}

