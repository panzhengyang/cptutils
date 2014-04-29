#include <stdlib.h>
#include <cptread.h>
#include "fixture.h"
#include "tests_cptread_helper.h"

extern cpt_t* load_cpt(const char* file)
{
  size_t n = 1024;
  char buf[n];

  if (fixture(buf, n, "cpt", file) < n)
    return NULL;

  cpt_t *cpt;

  if ((cpt = cpt_new()) == NULL)
    return NULL;

  if (cpt_read(buf, cpt) == 0)
    {
      return cpt;
    }
  else
    {
      cpt_destroy(cpt);
      return NULL;
    }
}
