/*
  convert a utf8 multibyte string to ascii/latin1 etc 
  with character transliteration

  $Id: utf8x.h,v 1.2 2011/11/04 15:16:50 jjg Exp jjg $
*/

#ifndef UTF8X_H
#define UTF8X_H

#include <stdlib.h>

extern int utf8_to_x(const char*, 
		     const unsigned char*, 
		     char*, size_t);

#endif
