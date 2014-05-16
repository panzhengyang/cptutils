/*
  tests_grd5string.h
  Copyright (c) J.J. Green 2014
*/

#include <grd5string.h>
#include "tests_grd5string.h"

CU_TestInfo tests_grd5string[] = 
  {
    {"matcher", test_grd5string_matches},
    CU_TEST_INFO_NULL,
  };

static grd5_string_t* build_grd5string(const char* content)
{
  grd5_string_t *str;

  if ((str = malloc(sizeof(grd5_string_t))) == NULL)
    return NULL;

  str->len = strlen(content);

  if ((str->content = malloc(str->len)) == NULL)
    return NULL;

  strncpy(str->content, content, str->len);

  return str;
}

extern void test_grd5string_matches(void)
{
  grd5_string_t *str;

  CU_TEST_FATAL( (str = build_grd5string("frooble")) != NULL );

  CU_ASSERT( grd5_string_matches(str, "frooble") == true  );
  CU_ASSERT( grd5_string_matches(str, "brooble") == false );

  grd5_string_destroy(str);
}


