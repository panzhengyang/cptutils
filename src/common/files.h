/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
  $Id: files.h,v 1.3 2011/11/11 15:40:25 jjg Exp $
*/

#ifndef FILES_H
#define FILES_H

extern int file_readable(const char*);
extern int absolute_filename(const char*);

#endif
