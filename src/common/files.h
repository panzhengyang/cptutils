/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.h,v 1.1 2002/06/18 22:25:44 jjg Exp jjg $
*/

#ifndef FILES_H
#define FILES_H

extern int file_readable(char*);
extern int absolute_filename(char*);

#endif
