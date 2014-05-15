/*
  cunit tests for grd3write.c
  J.J.Green 2014,
*/

#include <stdio.h>
#include <unistd.h>

#include <grd3write.h>
#include "tests_grd3write.h"

#include "fixture_grd3.h"

CU_TestInfo tests_grd3write[] = 
  {
    {"fixtures", test_grd3write_fixtures},
    {"no such directory", test_grd3write_nosuchdir},
    CU_TEST_INFO_NULL,
  };

/* write a selection of grd3 files */

extern void test_grd3write_fixtures(void)
{
  const char* files[] = {
    "accented.jgd",
    "test.1.jgd",
    "test.2.jgd",
    "test.3.jgd",
    "test.4.jgd"
  };
  size_t i, nfile = sizeof(files)/sizeof(char*);

  for (i=0 ; i<nfile ; i++)
    {
      grd3_t* grd3;
      CU_TEST_FATAL( (grd3 = load_grd3_fixture(files[i])) != NULL )

      char *path;
      CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL )

      CU_ASSERT( grd3_write(path, grd3) == 0 );

      unlink(path);
      grd3_destroy(grd3);
    }
}

/* write a grd3 to a non-existant directory */

extern void test_grd3write_nosuchdir(void)
{
  grd3_t* grd3;
  CU_TEST_FATAL( (grd3 = load_grd3_fixture("accented.jgd")) != NULL )

  CU_ASSERT(grd3_write("/no/such/directory/exists", grd3) != 0);

  grd3_destroy(grd3);
}
