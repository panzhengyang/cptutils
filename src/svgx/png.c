/*
  png structure for svgx
  J.J.Green
*/

#include <btrace.h>
#include "png.h"

extern png_t* png_new(size_t w, size_t h)
{
  png_t *png;

  if ((png = malloc(sizeof(png_t))) != NULL)
    {
      if ((png->row = malloc(4 * w)) != NULL)
	{
	  png->width  = w;
	  png->height = h;

	  return png;
	}
      else
	btrace("failed to allocate memory for png row");

      free(png);
    }
  else
    btrace("failed to allocate memory for png structure");

  return NULL;
}

extern void png_destroy(png_t* png)
{
  free(png->row);
  free(png);
}

