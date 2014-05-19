/*
  tests_ident.c
  Copyright (c) J.J. Green 2014
*/

#include <string.h>
#include <ident.h>
#include "tests_ident.h"

CU_TestInfo tests_ident[] = 
  {
    {"create", test_ident_create},
    CU_TEST_INFO_NULL,
  };

extern void test_ident_create(void)
{
  ident_t* ident;

  CU_TEST_FATAL( (ident = ident_new("foo", 27)) != NULL );
  CU_ASSERT( ident->id == 27 );
  CU_ASSERT( strcmp(ident->name, "foo") == 0 );

  ident_destroy(ident);
}

