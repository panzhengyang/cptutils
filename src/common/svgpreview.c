/*
  svgpreview.h

  specify a preview image in SVG output

  Copyright (c) J.J. Green 2012
*/

#include <stdio.h>
#include <string.h>

#include "svgpreview.h"

/*
  scan a string of the form <double>[/<double>] and fill in 
  the geometry section of the svg preview struct. the 
  preview->use should be true, else the string will not 
  be inspected (it might be NULL).
*/

#define MIN(x,y) (((x)<(y)) ? (x) : (y))

extern int svg_preview_geometry(const char *geom, svg_preview_t *preview)
{
  if (! preview->use) return 0;
  
  /* create non-const copy of geom */

  size_t n = strlen(geom); 
  char s[n+1]; 
  
  if (strncpy(s,geom,n+1) == NULL) 
    return 1;

  /* find seperator */

  char *sep = strchr(s,'x');

  if (sep)
    {
      /* split the string and scan each part */

      *sep = '\0';
      
      if (sscanf(s,"%zu",&(preview->width)) != 1)
	return 1;

      if (sscanf(sep+1,"%zu",&(preview->height)) != 1)
	return 1;
    }
  else
    {
      /* scan the whole string */

      if (sscanf(s,"%zu",&(preview->width)) != 0)
	return 1;

      preview->height = preview->width;
    }

  /* this is a bit ad-hoc, we could add another config option */

  size_t ssd = MIN(preview->height, preview->width);

  preview->border = 0.1 * ssd;
  preview->stroke = 1;

  return 0;
}
