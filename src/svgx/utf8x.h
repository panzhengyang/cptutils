/*
  convert a utf8 multibyte string to ascii/latin1 etc 
  with character transliteration
*/

#ifndef UTF8X_H
#define UTF8X_H

#include <stdlib.h>

extern int utf8_to_x(const char*, 
		     const unsigned char*, 
		     char*, size_t);

#endif
