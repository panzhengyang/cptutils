/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.c,v 1.1 2001/05/17 02:14:42 jjg Exp $
*/

#include <unistd.h>
#include "files.h"

extern int file_readable(char* name)
{
    return (access(name,R_OK) == 0);
}

extern  int absolute_filename(char* name)
{
    return (*name == DIRSEP);
}
