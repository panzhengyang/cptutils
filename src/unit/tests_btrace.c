/*
  tests_btrace.h
  Copyright (c) J.J. Green 2014
*/

#include <btrace.h>
#include "tests_btrace.h"

CU_TestInfo tests_btrace[] = 
  {
    {"default", test_btrace_default},
    {"enable",  test_btrace_enable},
    {"disable", test_btrace_disable},
    CU_TEST_INFO_NULL,
  };

/* btrace should be disabled by default */

extern void test_btrace_default(void)
{
  CU_ASSERT( btrace_is_enabled() == false );
}

/* btrace_enable() should change the enabled state */

extern void test_btrace_enable(void)
{
  CU_TEST_FATAL( btrace_is_enabled() == false );
  btrace_enable();
  CU_ASSERT( btrace_is_enabled() == true );
  btrace_disable();
  CU_TEST_FATAL( btrace_is_enabled() == false );
}

/* btrace_disable() should change the enabled state */

extern void test_btrace_disable(void)
{
  btrace_enable();
  CU_TEST_FATAL( btrace_is_enabled() == true );
  btrace_disable();
  CU_ASSERT( btrace_is_enabled() == false );
}
