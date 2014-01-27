/*
  grd5_type.c
  Copyright (c) J.J. Green 2014
*/

#include "grd5type.h"

#include <string.h>

/*
  return codes of grd5 keywords, just a switch on
  the first character and a strncmp() of the whole
  string -- one could use gperf here but there are
  only a few keywords to test.
*/

extern int grd5_type(const char* str)
{
  switch (str[0])
    {
    case 'b':
      if (strncmp("bool", str, 4) == 0)
	return TYPE_BOOL;
      break;
    case 'd':
      if (strncmp("doub", str, 4) == 0)
	return TYPE_DOUBLE;
      break;
    case 'l':
      if (strncmp("long", str, 4) == 0)
	return TYPE_LONG;
      break;
    case 'O':
      if (strncmp("Objc", str, 4) == 0)
	return TYPE_OBJECT;
      break;
    case 'T':
      if (strncmp("TEXT", str, 4) == 0)
	return TYPE_TEXT;
      break;
    case 'p':
      if (strncmp("patt", str, 4) == 0)
	return TYPE_PATTERN;
      break;
    case 'U':
      if (strncmp("Untf", str, 4) == 0)
	return TYPE_UntF;
      break;
    case 'V':
      if (strncmp("VlLs", str, 4) == 0)
	return TYPE_VlLs;
      break;
    default:
      return TYPE_UNKNOWN;
    }
  return TYPE_UNKNOWN;
}
