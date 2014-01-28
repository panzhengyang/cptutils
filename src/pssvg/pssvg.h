/*
  pssvg.h
  (c) J.J.Green 2014
*/

#ifndef PSSVG_H
#define PSSVG_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
  bool verbose, debug;
  char *name;
  struct
  {
    char *input, *output;
  } file;
} pssvg_opt_t;

extern int pssvg(pssvg_opt_t);

#endif



