#include <stdlib.h>
#include <grd3read.h>

#include "fixture.h"
#include "fixture_grd3.h"

extern grd3_t* load_grd3_fixture(const char* file)
{
  size_t n = 1024;
  char path[n];

  if (fixture(path, n, "grd3", file) >= n)
    return NULL;

  grd3_t *grd3;

  if ((grd3 = grd3_new()) == NULL)
    return NULL;

  if (grd3_read(path, grd3) != 0)
    {
      grd3_destroy(grd3);
      return NULL;
    }

  return grd3;
}
