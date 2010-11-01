/*
  gptwrite.h

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010\
  $Id: gptwrite.h,v 1.3 2006/08/27 23:04:06 jjg Exp $
*/

#ifndef GPTWRITE_H
#define GPTWRITE_H

#include "gpt.h"

extern int gpt_write(const char*,gpt_t*);

#endif
