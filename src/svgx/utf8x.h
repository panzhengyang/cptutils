/*
  convert a utf8 multibyte string to ascii with
  character transliteration

  $Id: utf8x.h,v 1.1 2011/11/03 23:43:14 jjg Exp jjg $
*/

#ifndef UTF8X_H
#define UTF8X_H

#include <stdlib.h>

extern int utf8_to_ascii(const char*, 
			 const unsigned char*, 
			 char*, size_t);

#endif
