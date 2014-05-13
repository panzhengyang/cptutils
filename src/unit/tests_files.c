/*
  tests_files.c
  Copyright (c) J.J. Green 2014
*/

#include <files.h>
#include "tests_files.h"

CU_TestInfo tests_files[] = 
  {
    {"file is readable", test_files_is_readable},
    {"path is absolute", test_files_is_absolute_filename},
    CU_TEST_INFO_NULL,
  };

extern void test_files_is_readable(void)
{

}

extern void test_files_is_absolute_filename(void)
{

}

