/*
  tests.c
  testcase loader
  J.J.Green 2014
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tests_btrace.h"
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
#include "tests_identtab.h"
#include "tests_pov.h"
#include "tests_povwrite.h"
#include "tests_sao.h"
#include "tests_saowrite.h"
#include "tests_stdcol.h"
#include "tests_svg.h"
#include "tests_svglist.h"
#include "tests_svgpreview.h"
#include "tests_svgread.h"
#include "tests_svgwrite.h"

#include <CUnit/CUnit.h>

static CU_SuiteInfo suites[] = 
  {
    { "btrace",     NULL, NULL, tests_btrace },
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
    { "identtab",   NULL, NULL, tests_identtab },
    { "pov",        NULL, NULL, tests_pov },
    { "povwrite",   NULL, NULL, tests_povwrite },
    { "sao",        NULL, NULL, tests_sao },
    { "saowrite",   NULL, NULL, tests_saowrite },
    { "stdcol",     NULL, NULL, tests_stdcol },
    { "svg",        NULL, NULL, tests_svg },
    { "svglist",    NULL, NULL, tests_svglist },
    { "svgpreview", NULL, NULL, tests_svgpreview },
    { "svgread",    NULL, NULL, tests_svgread },
    { "svgwrite",   NULL, NULL, tests_svgwrite },
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
