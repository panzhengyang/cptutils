/*
  tests_grd5type.h
  Copyright (c) J.J. Green 2014
*/

#include <grd5type.h>
#include "tests_grd5type.h"

CU_TestInfo tests_grd5type[] = 
  {
    {"lookup", test_grd5type_type},
    CU_TEST_INFO_NULL,
  };

extern void test_grd5type_type(void)
{
  CU_ASSERT( grd5_type("bool") == TYPE_BOOL );
  CU_ASSERT( grd5_type("doub") == TYPE_DOUBLE );
  CU_ASSERT( grd5_type("enum") == TYPE_ENUM );
  CU_ASSERT( grd5_type("long") == TYPE_LONG );
  CU_ASSERT( grd5_type("Objc") == TYPE_OBJECT );
  CU_ASSERT( grd5_type("tdta") == TYPE_TDTA );
  CU_ASSERT( grd5_type("TEXT") == TYPE_TEXT );
  CU_ASSERT( grd5_type("patt") == TYPE_PATTERN );
  CU_ASSERT( grd5_type("UntF") == TYPE_UNTF );
  CU_ASSERT( grd5_type("VlLs") == TYPE_VAR_LEN_LIST );
  CU_ASSERT( grd5_type("caca") == TYPE_UNKNOWN );
}
