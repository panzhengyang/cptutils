/*
  tests_povwrite.c
  Copyright (c) J.J. Green 2014
*/

#include <unistd.h>

#include <povwrite.h>
#include "tests_povwrite.h"

CU_TestInfo tests_povwrite[] =
  {
    {"two-stop example", test_povwrite_2stop },
    CU_TEST_INFO_NULL,
  };

static pov_t* build_pov(void)
{
  pov_t *pov;
  int changed;
  
  if ((pov = pov_new()) == NULL) 
    return NULL;

  if (pov_set_name(pov, "frooble", &changed) != 0)
    return NULL;

  if (pov_stops_alloc(pov, 2) != 0)
    return NULL;

  pov_stop_t stop[2] =
    {
      {0.0, {0.0, 0.0, 0.0, 0.0}},
      {1.0, {1.0, 0.5, 0.0, 0.0}},
    };
  int i;

  for (i=0 ; i<2 ; i++) pov->stop[i] = stop[i];

  return pov;
}

extern void test_povwrite_2stop(void)
{
  pov_t *pov;
  CU_TEST_FATAL( (pov = build_pov()) != NULL);

  char *path;
  CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL );

  CU_ASSERT( pov_write(path, pov) == 0 );
  CU_ASSERT( access(path, F_OK)   == 0 );

  unlink(path);

  pov_destroy(pov);
}

