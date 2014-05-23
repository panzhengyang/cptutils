/*
  tests_svg_helper.c
  Copyright (c) J.J. Green 2014
*/

#include "tests_svg_helper.h"

extern svg_t* build_svg(void)
{
  svg_t *svg;
  svg_stop_t stops[] = 
    {
      { 0.25, 0.5, {255,   0, 0}},
      { 0.75, 0.5, {  0, 255, 0}}
    };
  size_t nstop = sizeof(stops)/sizeof(svg_stop_t);

  if ( (svg = svg_new()) == NULL)
    return NULL;

  size_t i;

  for (i=0 ; i<nstop ; i++)
    if (svg_append(stops[i], svg) != 0)
      return NULL;
  
  return svg;
}
