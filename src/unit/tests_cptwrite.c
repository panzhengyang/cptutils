/*
  cunit tests for cptwrite.c
  J.J.Green 2014,
*/

#include <stdio.h>
#include <unistd.h>

#include <cptwrite.h>
#include "tests_cptwrite.h"

#include "fixture_cpt.h"

CU_TestInfo tests_cptwrite[] = 
  {
    {"fixtures", test_cptwrite_fixtures},
    {"no such directory", test_cptwrite_nosuchdir},
    CU_TEST_INFO_NULL,
  };

/* write a selection of cpt files */

extern void test_cptwrite_fixtures(void)
{
  const char* files[] = {
    "blue.cpt",
    "Exxon88.cpt",
    "GMT_gebco.cpt",
    "GMT_haxby.cpt",
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
      cpt_t* cpt = load_cpt_fixture(files[i]);
      char *path = tmpnam(NULL);

      CU_ASSERT(cpt_write(path, cpt) == 0);

      unlink(path);
      cpt_destroy(cpt);
    }
}

/* write a cpt to a non-existant directory */

extern void test_cptwrite_nosuchdir(void)
{
  cpt_t* cpt = load_cpt_fixture("blue.cpt");
  CU_ASSERT(cpt_write("/no/such/directory/exists", cpt) != 0);
  cpt_destroy(cpt);
}
