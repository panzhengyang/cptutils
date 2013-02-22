/*
  povwrite.h

  write a povray colour map struct to
  a file or stream.

  J.J.Green 2005
  $Id: povwrite.h,v 1.3 2006/08/27 23:04:06 jjg Exp $
*/

#ifndef POVWRITE_H
#define POVWRITE_H

#include "pov.h"

extern int pov_write(const char*,pov_t*);

#endif
