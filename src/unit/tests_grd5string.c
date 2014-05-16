/*
  tests_grd5string.h
  Copyright (c) J.J. Green 2014
*/

#include <grd5string.h>
#include "tests_grd5string.h"
#include "tests_grd5_helper.h"

CU_TestInfo tests_grd5string[] = 
  {
    {"matcher", test_grd5string_matches},
    CU_TEST_INFO_NULL,
  };

extern void test_grd5string_matches(void)
{
  grd5_string_t *str;

  CU_TEST_FATAL( (str = build_grd5string("frooble")) != NULL );

  CU_ASSERT( grd5_string_matches(str, "frooble") == true  );
  CU_ASSERT( grd5_string_matches(str, "brooble") == false );

  grd5_string_destroy(str);
}


