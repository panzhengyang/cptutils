/*
  tests_svglist.c
  Copyright (c) J.J. Green 2014
*/

#include <svglist.h>
#include "tests_svglist.h"
#include "tests_svglist_helper.h"

CU_TestInfo tests_svglist[] =
  {
    {"constructor", test_svglist_new},
    {"extract svg", test_svglist_svg},
    {"iterate",     test_svglist_iterate},
    {"select",      test_svglist_select},
    CU_TEST_INFO_NULL,
  };

extern void test_svglist_new(void)
{
  svg_list_t *svglist;

  CU_TEST_FATAL( (svglist = svg_list_new()) != NULL );

  svg_list_destroy(svglist);
}

extern void test_svglist_svg(void)
{
  svg_list_t *svglist;

  CU_TEST_FATAL( (svglist = svg_list_new()) != NULL );

  size_t i, n=7;

  for (i=0 ; i<n ; i++)
    {
      CU_ASSERT( svg_list_svg(svglist) != NULL );
      CU_ASSERT( svg_list_size(svglist) == i+1 );
    }

  svg_list_destroy(svglist);
}

static int count(svg_t *svg, int *n)
{
  (*n)++;

  return 0;
}

extern void test_svglist_iterate(void)
{
  svg_list_t *svglist;

  CU_TEST_FATAL( (svglist = build_svg_list(5)) != NULL);

  int n = 0;

  CU_ASSERT( svg_list_iterate(svglist, 
			      (int (*)(svg_t*, void*))count,
			      (void*)&n) == 0 );
  CU_ASSERT( n == 5 );

  svg_list_destroy(svglist);
}

static int break_third(svg_t *svg, int *n)
{
  return (++(*n) == 3);
}

extern void test_svglist_select(void)
{
  svg_list_t *svglist;

  CU_TEST_FATAL( (svglist = build_svg_list(50)) != NULL);

  int n = 0;
  svg_t *svg = svg_list_select(svglist, 
			       (int (*)(svg_t*, void*))break_third,
			       (void*)&n);
  CU_ASSERT( svg != NULL );

  CU_ASSERT( strcmp(svg->name, "id-02") == 0 );

  svg_list_destroy(svglist);
}
