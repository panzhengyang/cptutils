/*
  tests_grd5.h
  Copyright (c) J.J. Green 2014
*/

#include <grd5.h>
#include "tests_grd5.h"
#include "tests_grd5_helper.h"

CU_TestInfo tests_grd5[] =
  {
    {"model lookup", test_grd5_model},
    CU_TEST_INFO_NULL,
  };


extern void test_grd5_model(void)
{
  grd5_string_t *str;

  CU_TEST_FATAL( (str = build_grd5string("RGBC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_RGB );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("HSBC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_HSB );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("LbCl")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_LAB );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("CMYC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_CMYC );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("Grsc")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_GRSC );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("FrgC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_FRGC );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("BckC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_BCKC );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("BkCl")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_BOOK );
  grd5_string_destroy(str);

  CU_TEST_FATAL( (str = build_grd5string("UnsC")) != NULL);
  CU_ASSERT( grd5_model(str) == GRD5_MODEL_UNSC );
  grd5_string_destroy(str);
}
