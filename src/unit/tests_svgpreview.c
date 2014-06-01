/*
  tests_svgpreview.c
  Copyright (c) J.J. Green 2014
*/

#include <svgpreview.h>
#include "tests_svgpreview.h"

CU_TestInfo tests_svgpreview[] = 
  {
    {"geometry 50", test_svgpreview_geometry_50},
    {"geometry 50x40", test_svgpreview_geometry_50x40},
    {"geometry, use is false", test_svgpreview_geometry_false},
    CU_TEST_INFO_NULL,
  };

extern void test_svgpreview_geometry_50(void)
{
  svg_preview_t preview = { .use = true };
  CU_ASSERT( svg_preview_geometry("50", &preview) == 0 );
  CU_ASSERT( preview.width == 50 );
  CU_ASSERT( preview.height == 50 );
}

extern void test_svgpreview_geometry_50x40(void)
{
  svg_preview_t preview = { .use = true };
  CU_ASSERT( svg_preview_geometry("50x40", &preview) == 0 );
  CU_ASSERT( preview.width == 50 );
  CU_ASSERT( preview.height == 40 );
}

extern void test_svgpreview_geometry_false(void)
{
  svg_preview_t preview = { .use = false };
  CU_ASSERT( svg_preview_geometry(NULL, &preview) == 0 );
}
