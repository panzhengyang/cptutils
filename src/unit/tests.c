/*
  tests.c
  testcase loader
  J.J.Green 2014
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
#include "tests_ggr.h"
#include "tests_gmtcol.h"
#include "tests_gpt.h"
#include "tests_gptwrite.h"
#include "tests_grd3.h"
#include "tests_grd3read.h"
#include "tests_grd3write.h"
#include "tests_grd5.h"
#include "tests_grd5read.h"
#include "tests_grd5string.h"
#include "tests_grd5type.h"
#include "tests_grdxsvg.h"
#include "tests_gstack.h"
#include "tests_ident.h"

#include <CUnit/CUnit.h>

static CU_SuiteInfo suites[] = 
  {
    { "colour",     NULL, NULL, tests_colour },
    { "cpt",        NULL, NULL, tests_cpt },
    { "cptread",    NULL, NULL, tests_cptread },
    { "cptwrite",   NULL, NULL, tests_cptwrite },
    { "cptname",    NULL, NULL, tests_cptname },
    { "css3write",  NULL, NULL, tests_css3write }, 
    { "fill",       NULL, NULL, tests_fill },
    { "ggr",        NULL, NULL, tests_ggr },
    { "gmtcol",     NULL, NULL, tests_gmtcol },
    { "gpt",        NULL, NULL, tests_gpt },
    { "gptwrite",   NULL, NULL, tests_gptwrite },
    { "grd3",       NULL, NULL, tests_grd3 },
    { "grd3read",   NULL, NULL, tests_grd3read },
    { "grd3write",  NULL, NULL, tests_grd3write },
    { "grd5",       NULL, NULL, tests_grd5 },
    { "grd5read",   NULL, NULL, tests_grd5read },
    { "grd5string", NULL, NULL, tests_grd5string },
    { "grd5type",   NULL, NULL, tests_grd5type },
    { "grdxsvg",    NULL, NULL, tests_grdxsvg },
    { "gstack",     NULL, NULL, tests_gstack },
    { "ident",      NULL, NULL, tests_ident },
    CU_SUITE_INFO_NULL,
  };

void tests_load(void)
{
  assert(NULL != CU_get_registry());
  assert(!CU_is_test_running());

  if (CU_register_suites(suites) != CUE_SUCCESS) 
    {
      fprintf(stderr, "suite registration failed - %s\n",
	      CU_get_error_msg());
      exit(EXIT_FAILURE);
    }
}
