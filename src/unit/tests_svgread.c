/*
  cunit tests for svgread.c
  J.J.Green 2014,
*/

#include <svgread.h>
#include "fixture.h"
#include "tests_svgread.h"

CU_TestInfo tests_svgread[] = 
  {
    {"fixtures", test_svgread_fixtures},
    {"file does not exist", test_svgread_nofile},
    CU_TEST_INFO_NULL,
  };

extern void test_svgread_fixtures(void)
{
  size_t n = 1024;
  char buf[n];
  const char* files[] = {
    "BrBG_10.svg",
    "eyes.svg",
    "french-flag.svg",
    "gradient-pastel-blue.svg",
    "Gradients_01.svg",
    "lemon-lime.svg",
    "mad-ids.svg",
    "punaisa_01.svg",
    "radial-eclipse.svg",
    "red-green-blue.svg",
    "subtle.svg"
  };
  size_t i, nfile = sizeof(files)/sizeof(char*);

  for (i=0 ; i<nfile ; i++)
    {
      svg_list_t* svglist;

      CU_TEST_FATAL( (svglist = svg_list_new()) != NULL ); 
      CU_TEST_FATAL( fixture(buf, n, "svg", files[i]) < n );
      CU_ASSERT( svg_read(buf, svglist) == 0 );
      CU_ASSERT( svg_list_size(svglist) > 0 );
      svg_list_destroy(svglist);
    }
}

extern void test_svgread_nofile(void)
{
  svg_list_t* svglist;

  CU_TEST_FATAL( (svglist = svg_list_new()) != NULL ); 
  CU_ASSERT( svg_read("/tmp/no-such-file", svglist) != 0 );
  CU_ASSERT( svg_list_size(svglist) == 0 );
  svg_list_destroy(svglist);
}
