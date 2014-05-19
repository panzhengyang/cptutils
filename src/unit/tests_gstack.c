/*
  tests_gstack.h
  Copyright (c) J.J. Green 2014
*/

#include <gstack.h>
#include "tests_gstack.h"

CU_TestInfo tests_gstack[] = 
  {
    {"constructor", test_gstack_new},
    {"push/pop", test_gstack_pushpop},
    {"reverse", test_gstack_reverse},
    {"foreach", test_gstack_foreach},
    CU_TEST_INFO_NULL,
  };

extern void test_gstack_new(void)
{
  gstack_t* g;
  CU_TEST_FATAL( (g = gstack_new(4, 4, 4)) != NULL );
  CU_ASSERT( gstack_empty(g) == 1 );
  gstack_destroy(g);
}

static gstack_t* build_gstack(int n)
{
  gstack_t* g = gstack_new(sizeof(int), 10, 10);

  if (g == NULL)
    return NULL;

  int i;

  for (i=0 ; i<n ; i++)
    if (gstack_push(g, (void *) &i) != 0 )
      return NULL;

  return g;
}

extern void test_gstack_pushpop(void)
{
  gstack_t* g;

  CU_TEST_FATAL( (g = build_gstack(4)) != NULL );

  int i;

  for (i=0 ; i<4 ; i++)
    {
      int res;
      CU_ASSERT( gstack_empty(g) == 0 );
      CU_ASSERT( gstack_pop(g, (void *) &res) == 0 );
      CU_ASSERT( res ==  3 - i );
    }

  CU_ASSERT( gstack_empty(g) == 1 );

  gstack_destroy(g);
}

extern void test_gstack_reverse(void)
{
  gstack_t* g;

  CU_TEST_FATAL( (g = build_gstack(4)) != NULL );

  CU_ASSERT( gstack_size(g) == 4 );
  CU_ASSERT( gstack_reverse(g) == 0 );
  CU_ASSERT( gstack_size(g) == 4 );

  int i;

  for (i=0 ; i<4 ; i++)
    {
      int res;

      CU_ASSERT( gstack_empty(g) == 0 );
      CU_ASSERT( gstack_pop(g, (void *) &res) == 0 );
      CU_ASSERT( res ==  i );
    }

  CU_ASSERT( gstack_empty(g) == 1 );

  gstack_destroy(g);
}

static int add_cb(int *dat, int *sum)
{
  *sum += *dat;
  return 1;
}

extern void test_gstack_foreach(void)
{
  gstack_t* g;

  CU_TEST_FATAL( (g = build_gstack(4)) != NULL );

  int sum = 0;

  CU_ASSERT( gstack_foreach(g, (int (*)(void*, void*))add_cb, &sum) == 0 );
  CU_ASSERT( sum == 6 );

  gstack_destroy(g);
}
