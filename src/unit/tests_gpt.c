/*
  tests_gpt.h
  Copyright (c) J.J. Green 2014
*/

#include <gpt.h>
#include "tests_gpt.h"

CU_TestInfo tests_gpt[] = 
  {
    {"simple", test_gpt_simple},
    CU_TEST_INFO_NULL,
  };

extern void test_gpt_simple(void)
{
  gpt_t *gpt;

  CU_TEST_FATAL( (gpt = gpt_new()) != NULL );
  CU_ASSERT( gpt->n == 0 );
  CU_ASSERT( gpt->stop == NULL );
  CU_ASSERT( gpt_stops_alloc(gpt, 5) == 0 );
  CU_ASSERT( gpt->stop != NULL );
  CU_ASSERT( gpt->n == 5 );

  gpt_destroy(gpt);
}
