/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.h,v 1.1 2001/05/17 02:14:36 jjg Exp $
*/

#ifndef FILES_H
#define FILES_H

#define DIRSEP '/'

extern int file_readable(char*);
extern int absolute_filename(char*);

#endif
