/*
  cunit tests for cptname.c
  J.J.Green 2014
*/

#include <stdlib.h>
#include <cptname.h>
#include "tests_cptname.h"

CU_TestInfo tests_cptname[] = 
  {
    {"specific extension",      test_cptname_specific},
    {"wildcard extension",      test_cptname_wildcard},
    {"extension argument null", test_cptname_null},
    CU_TEST_INFO_NULL,
  };

/* with a specific extension */

extern void test_cptname_specific(void)
{
  char *name;

  name = cptname("base", "cpt");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.cpt", "cpt");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.cpt", "foo");
  CU_ASSERT(strcmp(name, "base.cpt") == 0);
  free(name);

  name = cptname("relative/directory/base.cpt", "cpt");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("/absolute/directory/base.cpt", "cpt");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);
}

/* with a wildcard extension */

extern void test_cptname_wildcard(void)
{
  char *name;

  name = cptname("base", "*");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.cpt", "*");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.foo", "*");
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.double.ext", "*");
  CU_ASSERT(strcmp(name, "base.double") == 0);
  free(name);
}

/* with a NULL extension */

extern void test_cptname_null(void)
{
  char *name;

  name = cptname("base", NULL);
  CU_ASSERT(strcmp(name, "base") == 0);
  free(name);

  name = cptname("base.cpt", NULL);
  CU_ASSERT(strcmp(name, "base.cpt") == 0);
  free(name);

  name = cptname("/foo/bar/base.cpt", NULL);
  CU_ASSERT(strcmp(name, "base.cpt") == 0);
  free(name);
}
