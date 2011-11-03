/*
  convert a utf8 multibyte string to ascii with
  character transliteration

  $Id$
*/

#ifndef UTF8ASCII_H
#define UTF8ASCII_H

#include <stdlib.h>

extern int utf8_to_ascii(const unsigned char*, char*, size_t);

#endif
