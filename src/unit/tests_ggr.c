/*
  tests_ggr.c
  Copyright (c) J.J. Green 2014
*/

#include <stdio.h>
#include <unistd.h>

#include <ggr.h>

#include "tests_ggr.h"

#include "fixture.h"
#include "fixture_ggr.h"

CU_TestInfo tests_ggr[] =
  {
    {"load",   test_ggr_load},
    {"save",   test_ggr_save},
    {"sample", test_ggr_sample},
    CU_TEST_INFO_NULL
  };

extern void test_ggr_load(void)
{
  size_t i, n = 1024;
  char path[n];

  const char *ggrs[] = {"Sunrise.ggr", "mars.ggr"};

  for (i = 0 ; i < 2  ; i++)
    {
      CU_TEST_FATAL( fixture(path, n, "ggr", ggrs[i]) < n );

      gradient_t *grad;
      CU_TEST_FATAL( (grad = grad_load_gradient(path)) != NULL );
      grad_free_gradient(grad);
    }
}

extern void test_ggr_save(void)
{
  gradient_t *ggr;
  CU_TEST_FATAL( (ggr = load_ggr_fixture("Sunrise.ggr")) != NULL );

  char *path = tmpnam(NULL);
  CU_ASSERT( grad_save_gradient(ggr, path) == 0 );

  unlink(path);
  grad_free_gradient(ggr);
}

extern void test_ggr_sample(void)
{
  /* FIXME */
}
