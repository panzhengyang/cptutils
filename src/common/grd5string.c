/*
  grd5string.h

  Copyright (c) J.J. Green 2014
*/

#include <stdlib.h>
#include <string.h>

#include "grd5string.h"

#include <stdio.h>

extern void grd5_string_destroy(grd5_string_t* gstr)
{
  if (gstr)
    {
      if ((gstr->len > 0) && (gstr->content))
	free(gstr->content);
      free(gstr);
    }
}

extern bool grd5_string_matches(grd5_string_t* gstr, const char* other)
{
  return strncmp(gstr->content, other, gstr->len) == 0;
}
