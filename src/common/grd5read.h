/*
  grd5read.h

  Read a grd5 (Photoshop gradient) file

  Copyright (c) J.J. Green 2014
  $Id$
*/

#ifndef GRD5READ_H
#define GRD5READ_H

#include "grd5.h"

#define GRD5_READ_OK    0
#define GRD5_READ_FOPEN 1
#define GRD5_READ_PARSE 2 
#define GRD5_READ_BUG   9 

extern int grd5_read(const char* file, grd5_t* grd5);

#endif
