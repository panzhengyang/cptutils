/*
  cunit tests for stdcol.c
  J.J.Green 2014
*/

#include <stdcol.h>
#include "tests_stdcol.h"

CU_TestInfo tests_stdcol[] = 
  {
    {"lookup from name", test_stdcol_lookup},
    {"name is not in list", test_stdcol_badname},
    CU_TEST_INFO_NULL,
  };

#define COLOUR "lightslategray"

extern void test_stdcol_lookup(void)
{
  struct stdcol_t* col;

  col = stdcol(COLOUR);

  if (col == NULL)
    {
      CU_FAIL("lookup returned NULL");
      return;
    }

  CU_ASSERT(strcmp(col->name, COLOUR) == 0);
  CU_ASSERT(col->r == 119);
  CU_ASSERT(col->g == 136);
  CU_ASSERT(col->b == 153);
}

extern void test_stdcol_badname(void)
{
  CU_ASSERT(stdcol("nosuchcolour") == NULL);
}
