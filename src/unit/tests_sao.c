/*
  tests_sao.h
  Copyright (c) J.J. Green 2014
*/

#include <sao.h>
#include "tests_sao.h"

CU_TestInfo tests_sao[] = 
  {
    {"create", test_sao_create },
    {"push colours", test_sao_push },
    {"colour iterator", test_sao_each },
    CU_TEST_INFO_NULL,
  };

extern void test_sao_create(void)
{
  sao_t *sao;

  CU_TEST_FATAL( (sao = sao_new()) != NULL);

  sao_destroy(sao);
}

extern void test_sao_push(void)
{
  sao_t *sao;

  CU_TEST_FATAL( (sao = sao_new()) != NULL);
  CU_ASSERT( sao_red_push(sao,   0, 0.0) == 0 );
  CU_ASSERT( sao_red_push(sao,   1, 0.0) == 0 );
  CU_ASSERT( sao_green_push(sao, 0, 0.0) == 0 );
  CU_ASSERT( sao_green_push(sao, 1, 0.5) == 0 );
  CU_ASSERT( sao_blue_push(sao,  0, 0.0) == 0 );
  CU_ASSERT( sao_blue_push(sao,  1, 1.0) == 0 );

  sao_destroy(sao);
}

static int sum_stop_fn(double z, double red, double *A)
{
  *A += red;
  return 0;
}

extern void test_sao_each(void)
{
  sao_t *sao;

  CU_TEST_FATAL( (sao = sao_new()) != NULL);
  CU_ASSERT( sao_red_push(sao,  0.0, 0.25) == 0 );
  CU_ASSERT( sao_red_push(sao,  0.5, 0.50) == 0 );
  CU_ASSERT( sao_red_push(sao,  1.0, 0.25) == 0 );

  double sum = 0.0;

  CU_ASSERT( sao_eachred(sao, (stop_fn_t*)sum_stop_fn, 
			 (void*)&sum) == 0 );
  CU_ASSERT( sum == 1.0 );
}
