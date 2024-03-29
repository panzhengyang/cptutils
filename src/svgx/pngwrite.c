/*
  png structure for svgx
  J.J.Green
*/

#include <png.h>
#include <btrace.h>
#include "pngwrite.h"

extern int png_write(const char *path, png_t *png, const char* name)
{
  png_structp pngH = NULL;
  png_infop   infoH = NULL;

  png_byte **rows = NULL;
  
  pngH = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (pngH == NULL)
    {
      btrace("failed to create png write struct");
      return 1;
    }    

  if ((infoH = png_create_info_struct(pngH)) == NULL)
    {
      btrace("failed to create png info struct");
      return 1;
    }

  if (setjmp(png_jmpbuf(pngH)))
    {
      btrace("failed to set lonjump");
      return 1;
    }
    
  /* Set image attributes. */

  png_set_IHDR(pngH,
	       infoH,
	       png->width,
	       png->height,
	       8,
	       PNG_COLOR_TYPE_RGBA,
	       PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT,
	       PNG_FILTER_TYPE_DEFAULT);

  /*
    since all rows are identical, using PNG_FILTER_UP 
    gives us better compression
  */

  png_set_filter(pngH, PNG_FILTER_TYPE_BASE, PNG_FILTER_UP);

  /*
    initialize rows of PNG, assigning each to the same
    simple row
  */

  if ((rows = malloc(png->height * sizeof(png_byte*))) == NULL)
    {
      btrace("failed to allocate memory");
      return 1;
    }

  size_t j;

  for (j = 0 ; j < png->height ; j++) rows[j] = png->row;
    
  /* Write the image data */

  FILE *st;

  if ((st = fopen(path, "wb")) == NULL) 
    return 1;

  png_init_io(pngH, st);
  png_set_rows(pngH, infoH, rows);
  png_write_png(pngH, infoH, PNG_TRANSFORM_IDENTITY, NULL);

  fclose(st);

  /* tidy up */

  free(rows);
  png_destroy_write_struct(&pngH, &infoH);

  return 0;
}
