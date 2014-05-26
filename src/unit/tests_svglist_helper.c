/*
  tests_svglist_helper.c
  Copyright (c) J.J. Green 2014
*/

#include "tests_svglist_helper.h"

extern svg_list_t* build_svg_list(size_t n)
{
  svg_list_t *svglist;

  if ((svglist = svg_list_new()) == NULL)
    return NULL;

  size_t i;

  for (i=0 ; i<n ; i++)
    {
      svg_t *svg;

      if ( (svg = svg_list_svg(svglist)) == NULL )
	return NULL;

      sprintf(svg->name, "id-%02d", i);
    }

  return svglist;
}
