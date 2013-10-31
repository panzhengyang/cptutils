/*
  cptname.c

  extract a basename from a path

  Copyright (c) J.J. Green 2013
  $Id$
*/

#include <string.h>

#include "cptname.h"

#ifndef DIRSEP
#define DIRSEP '/'
#endif

/*
  Return the basename of a path (the part after the last
  DIRSEP in the path, or the whole path).  If the path
  extension is as given in the last argument, it will be 
  also be stripped. If the last argument is "*" then any
  extension will be stripped.

  This was originally "basename", but that clashed with 
  a function in GNU libc
*/

extern char* cptname(const char *path, const char *ext)
{
  const char *pb;

  if ((pb = strrchr(path, '/')) != NULL)
    pb++;
  else
    pb = path;

  char *pc;

  if ((pc = strdup(pb)) == NULL)
    return NULL;

  if (ext)
    {
      char *px;

      if ((px = strrchr(pc, '.')) != NULL)
        {
          if ((strcmp("*", ext) == 0) || (strcmp(px + 1, ext) == 0))
	    *px = '\0';
        }
    }

  return pc;
}

