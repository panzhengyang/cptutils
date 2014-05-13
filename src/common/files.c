/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
*/

#include <unistd.h>
#include "files.h"

extern int is_readable(const char* name)
{
    return access(name, R_OK) == 0;
}

extern  int is_absolute_filename(const char* name)
{
    return *name == '/';
}
