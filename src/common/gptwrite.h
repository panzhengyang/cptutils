/*
  gptwrite.h

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010\
  $Id: gptwrite.h,v 1.1 2010/11/01 18:42:56 jjg Exp $
*/

#ifndef GPTWRITE_H
#define GPTWRITE_H

#include "gpt.h"

extern int gpt_write(const char*,gpt_t*);

#endif
