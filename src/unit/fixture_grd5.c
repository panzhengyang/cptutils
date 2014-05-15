#include <stdlib.h>
#include <grd5read.h>

#include "fixture.h"
#include "fixture_grd5.h"

extern grd5_t* load_grd5_fixture(const char* file)
{
  size_t n = 1024;
  char path[n];

  if (fixture(path, n, "grd5", file) >= n)
    return NULL;

  grd5_t *grd5;

  if (grd5_read(path, &grd5) != GRD5_READ_OK)
    return NULL;

  return grd5;
}
