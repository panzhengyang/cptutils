/*
  tests_grd5.h
  Copyright (c) J.J. Green 2014
*/

#include <grd5.h>
#include "tests_grd5.h"

CU_TestInfo tests_grd5[] = 
  {
    {"model lookup", test_grd5_model},
    CU_TEST_INFO_NULL,
  };


static void build_grd5_string(const char* content, grd5_string_t* str)
{
  str->content = (char*)content;
  str->len = strlen(content);
}

extern void test_grd5_model(void)
{
  grd5_string_t str;

  build_grd5_string("RGBC", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_RGB );

  build_grd5_string("HSBC", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_HSB );

  build_grd5_string("LbCl", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_LAB );

  build_grd5_string("CMYC", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_CMYC );

  build_grd5_string("Grsc", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_GRSC );

  build_grd5_string("FrgC", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_FRGC );

  build_grd5_string("BckC", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_BCKC );

  build_grd5_string("BkCl", &str);
  CU_ASSERT( grd5_model(&str) == GRD5_MODEL_BOOK );
}


