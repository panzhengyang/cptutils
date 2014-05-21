/*
  tests_saowrite.c
  Copyright (c) J.J. Green 2014
*/

#include <stdio.h>
#include <unistd.h>

#include <saowrite.h>
#include "tests_saowrite.h"
#include "tests_sao_helper.h"

CU_TestInfo tests_saowrite[] = 
  {
    {"write",             test_saowrite_write},
    {"no such directory", test_saowrite_nosuchdir},
    CU_TEST_INFO_NULL,
  };

extern void test_saowrite_write(void)
{
  sao_t* sao;
  CU_TEST_FATAL( (sao = build_sao()) != NULL );

  char *path;
  CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL );

  CU_ASSERT( access(path, F_OK) != 0 );
  CU_ASSERT( sao_write(path, sao, "froob") == 0 );
  CU_ASSERT( access(path, F_OK) == 0 );

  unlink(path);

  sao_destroy(sao);
}

extern void test_saowrite_nosuchdir(void)
{
  sao_t* sao;
  CU_TEST_FATAL( (sao = build_sao()) != NULL );

  const char path[] = "/no/such/directory";

  CU_ASSERT( access(path, F_OK)   != 0 );
  CU_ASSERT( sao_write(path, sao, "groob") != 0 );
  CU_ASSERT( access(path, F_OK)   != 0 );

  sao_destroy(sao);
}
