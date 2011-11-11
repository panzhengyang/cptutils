/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.c,v 1.2 2004/01/30 00:08:48 jjg Exp jjg $
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
