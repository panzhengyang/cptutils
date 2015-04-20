/*
  cunit tests for gmtcol.c
  J.J.Green 2014
*/

#include <gmtcol.h>
#include "tests_gmtcol.h"

CU_TestInfo tests_gmtcol[] = 
  {
    {"lookup from name", test_gmtcol_lookup},
    {"name is not in list", test_gmtcol_badname},
    CU_TEST_INFO_NULL,
  };

#define COLOUR "lightslategray"

extern void test_gmtcol_lookup(void)
{
  const struct gmtcol_t* col;

  col = gmtcol(COLOUR);

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

extern void test_gmtcol_badname(void)
{
  CU_ASSERT(gmtcol("nosuchcolour") == NULL);
}
