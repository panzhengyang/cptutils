/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.c,v 1.3 2011/11/11 15:40:18 jjg Exp $
*/

#include <unistd.h>
#include "files.h"

extern int file_readable(const char* name)
{
    return (access(name,R_OK) == 0);
}

extern  int absolute_filename(const char* name)
{
    return (*name == '/');
}
