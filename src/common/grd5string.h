/*
  grd5string.h

  Copyright (c) J.J. Green 2014
*/

#ifndef GRD5STRING_H
#define GRD5STRING_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint32_t len;
  char *content;
} grd5_string_t;

extern void grd5_string_destroy(grd5_string_t* gstr);
extern bool grd5_string_matches(grd5_string_t* gstr, const char* other);

#endif
