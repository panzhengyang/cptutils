/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.c,v 1.1 2002/06/18 22:25:32 jjg Exp jjg $
*/

#include <unistd.h>
#include "files.h"

extern int file_readable(char* name)
{
    return (access(name,R_OK) == 0);
}

extern  int absolute_filename(char* name)
{
    return (*name == '/');
}
