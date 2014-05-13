/*
  files.h

  Most of the portability problems of dealing with
  files are moved to this directory.

  (c) J.J.Green 2001
*/

#ifndef FILES_H
#define FILES_H

extern int is_readable(const char*);
extern int is_absolute_filename(const char*);

#endif
