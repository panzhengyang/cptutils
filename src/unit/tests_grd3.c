/*
  tests_grd3.h
  Copyright (c) J.J. Green 2014
*/

#include <grd3.h>
#include "tests_grd3.h"

CU_TestInfo tests_grd3[] = 
  {
    {"constructor", test_grd3_new},
    CU_TEST_INFO_NULL,
  };

extern void test_grd3_new(void)
{
  grd3_t *grd3;

  CU_TEST_FATAL( (grd3 = grd3_new()) != NULL);

  CU_ASSERT( grd3->name    == NULL );
  CU_ASSERT( grd3->rgb.n   == 0    );
  CU_ASSERT( grd3->rgb.seg == NULL );
  CU_ASSERT( grd3->op.n    == 0    );
  CU_ASSERT( grd3->op.seg  == NULL );

  grd3_destroy(grd3);
}


