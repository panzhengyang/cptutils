/*
  cunit tests for cptread.c
  J.J.Green 2014,
*/

#include <cptread.h>
#include "fixture.h"
#include "tests_cptread.h"

CU_TestInfo tests_cptread[] =
  {
    {"fixtures",            test_cptread_fixtures},
    {"file does not exist", test_cptread_nofile},
    CU_TEST_INFO_NULL,
  };

extern void test_cptread_fixtures(void)
{
  size_t n = 1024;
  char buf[n];
  const char* files[] = {
    "blue.cpt",
    "Exxon88.cpt",
    "GMT_gebco.cpt",
    "GMT_haxby.cpt",
    "GMT_cyclic.cpt",
    "Onion_Rings.cpt",
    "pakistan.cpt",
    "RdBu_10.cpt",
    "subtle.cpt",
    "test.cpt",
    "tpsfhm.cpt"
  };
  size_t i, nfile = sizeof(files)/sizeof(char*);

  for (i=0 ; i<nfile ; i++)
    {
      cpt_t* cpt;

      CU_ASSERT((cpt = cpt_new()) != NULL);
      CU_ASSERT(fixture(buf, n, "cpt", files[i]) < n);
      CU_ASSERT(cpt_read(buf, cpt) == 0);
      cpt_destroy(cpt);
    }
}

extern void test_cptread_nofile(void)
{
  cpt_t* cpt;

  CU_ASSERT((cpt = cpt_new()) != NULL);
  CU_ASSERT(cpt_read("/tmp/no-such-file", cpt) != 0);
  cpt_destroy(cpt);
}
