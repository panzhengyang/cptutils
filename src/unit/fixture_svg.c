#include <stdlib.h>
#include <svgread.h>

#include "fixture.h"
#include "fixture_svg.h"

extern svg_list_t* load_svg_fixture(const char* file)
{
  size_t n = 1024;
  char path[n];

  if (fixture(path, n, "svg", file) >= n)
    return NULL;

  svg_list_t *svglist;

  if ((svglist = svg_list_new()) == NULL)
    return NULL;

  if (svg_read(path, svglist) != 0)
    return NULL;

  return svglist;
}
