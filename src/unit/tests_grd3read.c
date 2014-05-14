/*
  cunit tests for grd3read.c
  J.J.Green 2014,
*/

#include <grd3read.h>
#include "fixture.h"
#include "tests_grd3read.h"

CU_TestInfo tests_grd3read[] = 
  {
    {"fixtures",            test_grd3read_fixtures},
    {"file does not exist", test_grd3read_nofile},
    CU_TEST_INFO_NULL,
  };

extern void test_grd3read_fixtures(void)
{
  size_t n = 1024;
  char buf[n];
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

      CU_TEST_FATAL( (grd3 = grd3_new()) != NULL ); 
      CU_TEST_FATAL( fixture(buf, n, "grd3", files[i]) < n );
      CU_ASSERT(grd3_read(buf, grd3) == 0);
      grd3_destroy(grd3);
    }
}

extern void test_grd3read_nofile(void)
{
  grd3_t* grd3;

  CU_ASSERT((grd3 = grd3_new()) != NULL); 
  CU_ASSERT(grd3_read("/tmp/no-such-file.PspGradient", grd3) != 0);
  grd3_destroy(grd3);
}

