/*
  tests.c
  testcase loader
  J.J.Green 2014
  $Id: tests.c,v 1.10 2008/09/10 22:34:42 jjg Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tests_colour.h"
#include "tests_cpt.h"
#include "tests_cptread.h"
#include "tests_cptwrite.h"
#include "tests_cptname.h"
#include "tests_css3write.h"
#include "tests_fill.h"
#include "tests_files.h"
#include "tests_gmtcol.h"

#include <CUnit/CUnit.h>

static CU_SuiteInfo suites[] = 
  {
    { "colour",    NULL, NULL, tests_colour },
    { "cpt",       NULL, NULL, tests_cpt },
    { "cptread",   NULL, NULL, tests_cptread },
    { "cptwrite",  NULL, NULL, tests_cptwrite },
    { "cptname",   NULL, NULL, tests_cptname },
    { "css3write", NULL, NULL, tests_css3write }, 
    { "fill",      NULL, NULL, tests_fill },
    { "files",     NULL, NULL, tests_files },
    { "gmtcol",    NULL, NULL, tests_gmtcol },
    CU_SUITE_INFO_NULL,
  };

void tests_load(void)
{
  assert(NULL != CU_get_registry());
  assert(!CU_is_test_running());

  if (CU_register_suites(suites) != CUE_SUCCESS) 
    {
      fprintf(stderr,"suite registration failed - %s\n",
	      CU_get_error_msg());
      exit(EXIT_FAILURE);
    }
}
