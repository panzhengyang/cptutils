/*
  povwrite.h

  write a povray colour map struct to
  a file or stream.

  J.J.Green 2005
  $Id: povwrite.h,v 1.1 2005/08/29 21:13:58 jjg Exp jjg $
*/

#ifndef POVWRITE_H
#define POVWRITE_H

#include "pov.h"

extern int pov_write(const char*,pov_t*,int*);

#endif
