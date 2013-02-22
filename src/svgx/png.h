/*
  png structures for cptutils
  J.J.Green
  $Id: png.h,v 1.1 2011/11/13 21:16:52 jjg Exp $
*/

/* note that PNG_H is defined by libpng */

#ifndef PNG2_H
#define PNG2_H

#include <stdlib.h>

typedef struct
{
  size_t width, height;
  unsigned char *row;
} png_t;

extern png_t* png_new(size_t, size_t);
extern void png_destroy(png_t*);

#endif
