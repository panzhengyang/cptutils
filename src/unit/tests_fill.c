/*
  cunit tests for cpt.c
  J.J.Green 2014,
*/

#include <fill.h>
#include "tests_fill_helper.h"
#include "tests_fill.h"

CU_TestInfo tests_fill[] = 
  {
    {"no-op", test_fill_noop},
    CU_TEST_INFO_NULL,
  };

extern void test_fill_noop(void)
{

}
