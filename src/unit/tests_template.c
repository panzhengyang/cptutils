/*
  cunit tests for template.c
  J.J.Green 2014
*/

#include "template.h"

CU_TestInfo tests_template[] = 
  {
    {"Template test, always passes", test_template_noop},
    CU_TEST_INFO_NULL,
  };

extern void test_template_noop(void)
{
  // nothing doing
}
