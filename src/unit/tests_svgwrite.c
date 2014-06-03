/*
  cunit tests for svgwrite.c
  J.J.Green 2014
*/

#include <stdio.h>
#include <unistd.h>

#include <svgwrite.h>
#include "tests_svgwrite.h"

#include "fixture_svg.h"

CU_TestInfo tests_svgwrite[] = 
  {
    {"fixtures", test_svgwrite_fixtures},
    {"no such directory", test_svgwrite_nosuchdir},
    CU_TEST_INFO_NULL,
  };

/* write a selection of svg files */

static int writetmp(svg_t *svg, svg_preview_t *preview)
{
  char *path = tmpnam(NULL);

  CU_ASSERT( access(path, F_OK) != 0 );
  CU_ASSERT( svg_write(path, 1, (const svg_t**)(&svg), preview) == 0 );
  CU_ASSERT( access(path, F_OK) == 0 );

  unlink(path);

  return 0;
} 

extern void test_svgwrite_fixtures(void)
{
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
  svg_preview_t preview = { .use = true };

  CU_TEST_FATAL( svg_preview_geometry("50x40", &preview) == 0 );

  for (i=0 ; i<nfile ; i++)
    {
      svg_list_t* svglist;

      CU_TEST_FATAL( (svglist = load_svg_fixture(files[i])) != NULL );
      CU_ASSERT( svg_list_iterate(svglist, 
				  (int (*)(svg_t*, void*))writetmp, 
				  (void*)&preview) == 0 );

      svg_list_destroy(svglist);
    }
}

/* write a svg to bad path  */

static int nosuchdir(svg_t *svg, svg_preview_t *preview)
{
  const char path[] = "/no/such/directory/exists";

  CU_ASSERT( access(path, F_OK) != 0 );
  CU_ASSERT( svg_write(path, 1, (const svg_t**)(&svg), preview) != 0 );

  return 0;
} 

extern void test_svgwrite_nosuchdir(void)
{
  svg_list_t* svglist;
  svg_preview_t preview = { .use = true };

  CU_TEST_FATAL( (svglist = load_svg_fixture("Gradients_01.svg")) != NULL );
  CU_TEST_FATAL( svg_preview_geometry("50x40", &preview) == 0 );

  CU_ASSERT( svg_list_iterate(svglist, 
			      (int (*)(svg_t*, void*))nosuchdir, 
			      (void*)&preview) == 0 );

  svg_list_destroy(svglist);
}
