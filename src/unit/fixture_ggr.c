#include <stdlib.h>
#include "fixture.h"
#include "fixture_ggr.h"

extern gradient_t* load_ggr_fixture(const char* file)
{
  size_t n = 1024;
  char path[n];

  if (fixture(path, n, "ggr", file) >= n)
    return NULL;

  return grad_load_gradient(path);
}
