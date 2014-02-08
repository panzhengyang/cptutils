/*
  ucs2utf8.h
  Copyright (c) J.J. Green 2014
*/

#ifndef UCS2UTF8_H
#define UCS2UTF8_H

extern int ucs2_to_utf8(const char *ucs2, size_t ucs2len,
			char *utf8, size_t utf8len);

#endif
