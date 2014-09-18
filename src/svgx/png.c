/*
  png structure for svgx
  J.J.Green
*/

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

      free(png);
    }
 
  return NULL;
}

extern void png_destroy(png_t* png)
{
  free(png->row);
  free(png);
}

