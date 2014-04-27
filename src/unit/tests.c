/*
  tests.c
  testcase loader
  J.J.Green 2007
  $Id: tests.c,v 1.10 2008/09/10 22:34:42 jjg Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tests-cpt.h"

#include <CUnit/CUnit.h>

static CU_SuiteInfo suites[] = 
  {
    { "cpt", NULL, NULL, tests_cpt },
    CU_SUITE_INFO_NULL,
  };

void tests_load(void)
{
  assert(NULL != CU_get_registry());
  assert(!CU_is_test_running());

  if (CU_register_suites(suites) != CUE_SUCCESS) 
    {
      fprintf(stderr,"suite registration failed - %s\n",
	      CU_get_error_msg());
      exit(EXIT_FAILURE);
    }
}
