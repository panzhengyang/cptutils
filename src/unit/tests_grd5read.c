/*
  cunit tests for grd5read.c
  J.J.Green 2014,
*/

#include <grd5read.h>
#include "fixture.h"
#include "tests_grd5read.h"

CU_TestInfo tests_grd5read[] = 
  {
    {"fixtures",            test_grd5read_fixtures},
    {"file does not exist", test_grd5read_nofile},
    CU_TEST_INFO_NULL,
  };

extern void test_grd5read_fixtures(void)
{
  size_t n = 1024;
  char buf[n];
  const char* files[] = {
    "my-custom-gradient-3-rgb.grd",
    "my-custom-gradient-3-hsb.grd",
    "my-custom-gradient-3-lab.grd",
    "neverhappens-titi-montoya.grd"
  };
  size_t i, nfile = sizeof(files)/sizeof(char*);

  for (i=0 ; i<nfile ; i++)
    {
      grd5_t* grd5;

      CU_TEST_FATAL( fixture(buf, n, "grd5", files[i]) < n );
      CU_TEST_FATAL( grd5_read(buf, &grd5) == 0 );
      grd5_destroy(grd5);
    }
}

extern void test_grd5read_nofile(void)
{
  grd5_t* grd5 = NULL;

  CU_ASSERT( grd5_read("/tmp/no-such-file.grd", &grd5) != 0 );
  CU_ASSERT( grd5 == NULL );
}

