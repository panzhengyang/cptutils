/*
  tests_gptwrite.c
  Copyright (c) J.J. Green 2014
*/

#include <stdio.h>
#include <unistd.h>

#include <gptwrite.h>
#include "tests_gptwrite.h"

CU_TestInfo tests_gptwrite[] = 
  {
    {"write", test_gptwrite_write},
    {"no such directory", test_gptwrite_nosuchdir},
    CU_TEST_INFO_NULL,
  };

static gpt_t* build_gpt(void)
{
  gpt_stop_t stops[] = {
    {0.0, {0.0, 0.0, 0.0}},
    {1.0, {0.0, 0.5, 1.0}}
  };
  size_t i, n = sizeof(stops) / sizeof(gpt_stop_t);
  gpt_t *gpt;

  if ((gpt = gpt_new()) == NULL)
    return NULL;
  
  if (gpt_stops_alloc(gpt, n) != 0)
    return NULL;

  for (i=0 ; i<n ; i++)
    gpt->stop[i] = stops[i];

  return gpt;
}

extern void test_gptwrite_write(void)
{
  gpt_t* gpt;
  CU_TEST_FATAL( (gpt = build_gpt()) != NULL );

  char *path;
  CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL );

  CU_ASSERT( access(path, F_OK)   != 0 );
  CU_ASSERT( gpt_write(path, gpt) == 0 );
  CU_ASSERT( access(path, F_OK)   == 0 );

  gpt_destroy(gpt);
}

extern void test_gptwrite_nosuchdir(void)
{
  gpt_t* gpt;
  CU_TEST_FATAL( (gpt = build_gpt()) != NULL );

  const char path[] = "/no/such/directory";

  CU_ASSERT( access(path, F_OK)   != 0 );
  CU_ASSERT( gpt_write(path, gpt) != 0 );
  CU_ASSERT( access(path, F_OK)   != 0 );

  gpt_destroy(gpt);
}

