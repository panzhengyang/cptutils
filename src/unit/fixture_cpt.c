#include <stdlib.h>
#include <cptread.h>
#include "fixture.h"
#include "fixture_cpt.h"

extern cpt_t* load_cpt_fixture(const char* file)
{
  size_t n = 1024;
  char path[n];

  if (fixture(path, n, "cpt", file) >= n)
    return NULL;

  cpt_t *cpt;

  if ((cpt = cpt_new()) == NULL)
    return NULL;

  if (cpt_read(path, cpt) != 0)
    {
      cpt_destroy(cpt);
      return NULL;
    }

  return cpt;
}
