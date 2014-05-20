/*
  tests_pov.c
  Copyright (c) J.J. Green 2014
*/

#include <pov.h>
#include "tests_pov.h"

CU_TestInfo tests_pov[] = 
  {
    {"create", test_pov_create},
    {"set name (simple)", test_pov_set_name_simple},
    {"set name (complex)", test_pov_set_name_complex},
    {"allocate stops", test_pov_alloc_stops},
    CU_TEST_INFO_NULL,
  };

extern void test_pov_create(void)
{
  pov_t *pov;

  CU_TEST_FATAL( (pov = pov_new()) != NULL);

  CU_ASSERT( pov->n == 0 );
  CU_ASSERT( pov->stop == NULL );
  CU_ASSERT( pov->name[0] == '\0' );

  pov_destroy(pov);
}

extern void test_pov_set_name_simple(void)
{
  pov_t *pov;
  int changed = 0;

  CU_TEST_FATAL( (pov = pov_new()) != NULL);

  CU_ASSERT( pov_set_name(pov, "frooble", &changed) == 0 );
  CU_ASSERT( changed == 0 );
  CU_ASSERT( strcmp(pov->name, "frooble") == 0 );

  pov_destroy(pov);
}

extern void test_pov_set_name_complex(void)
{
  pov_t *pov;
  int changed = 0;

  CU_TEST_FATAL( (pov = pov_new()) != NULL);

  CU_ASSERT( pov_set_name(pov, "bip bip b*p", &changed) == 0 );
  CU_ASSERT( changed == 3 );
  CU_ASSERT( strcmp(pov->name, "bip_bip_b_p") == 0 );

  pov_destroy(pov);
}

extern void test_pov_alloc_stops(void)
{
  pov_t *pov;

  CU_TEST_FATAL( (pov = pov_new()) != NULL);

  CU_ASSERT( pov_stops_alloc(pov, 5) == 0 );
  CU_ASSERT( pov->n == 5 );
  CU_ASSERT( pov->stop != NULL );

  pov_destroy(pov);
}
