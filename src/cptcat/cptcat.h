/*
  cptcat.h

  Copyright (c) J.J. Green 2013
*/

#ifndef CPTCAT_H
#define CPTCAT_H

#include "fill.h"

typedef struct 
{
  int verbose, n;
  rgb_t *fg, *bg, *nan;
  const char *output;
  struct
  {
    int n;
    const char **file;
  } input;
} cptcat_opt_t;

extern int cptcat(cptcat_opt_t opt);

#endif
