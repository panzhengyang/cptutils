/*
  tests_svg.c
  Copyright (c) J.J. Green 2014
*/

#include <svg.h>
#include "tests_svg.h"
#include "tests_svg_helper.h"
#include "tests_colour_helper.h"

CU_TestInfo tests_svg[] = 
  {
    {"create",        test_svg_new },
    {"append",        test_svg_append },
    {"prepend",       test_svg_prepend },
    {"stop iterator", test_svg_each_stop },
    {"stop count",    test_svg_num_stops },
    {"add explicit",  test_svg_explicit },
    {"interpolate",   test_svg_interpolate },
    {"flatten",       test_svg_flatten },
    CU_TEST_INFO_NULL,
  };

extern void test_svg_new(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = svg_new()) != NULL);

  CU_ASSERT( svg->nodes == NULL );
  CU_ASSERT( svg->name != NULL );
  CU_ASSERT( svg_num_stops(svg) == 0 );

  svg_destroy(svg);
}

extern void test_svg_append(void)
{
  svg_t *svg;
  svg_stop_t stopA = { 0.25, 1.0, {255,   0, 0}};
  svg_stop_t stopB = { 0.75, 1.0, {  0, 255, 0}};
    
  CU_TEST_FATAL( (svg = svg_new()) != NULL);

  CU_ASSERT( svg_append(stopA, svg) == 0 );
  CU_ASSERT( svg_append(stopB, svg) == 0 );
  CU_ASSERT( svg_num_stops(svg) == 2 );

  svg_destroy(svg);
}

extern void test_svg_prepend(void)
{
  svg_t *svg;
  svg_stop_t stopA = { 0.25, 1.0, {255,   0, 0}};
  svg_stop_t stopB = { 0.75, 1.0, {  0, 255, 0}};
    
  CU_TEST_FATAL( (svg = svg_new()) != NULL);

  CU_ASSERT( svg_prepend(stopB, svg) == 0 );
  CU_ASSERT( svg_prepend(stopA, svg) == 0 );
  CU_ASSERT( svg_num_stops(svg) == 2 );

  svg_destroy(svg);
}

static int val_sum(svg_stop_t *stop, double *sum)
{
  *sum += stop->value;

  return 0;
}

extern void test_svg_each_stop(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = build_svg()) != NULL);

  double sum = 0;

  CU_ASSERT( svg_each_stop(svg, 
			   (int (*)(svg_stop_t*, void*))val_sum, 
			   &sum) == 0 ); 
  CU_ASSERT( sum == 1.0 );

  svg_destroy(svg);
}

extern void test_svg_num_stops(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = build_svg()) != NULL);
  CU_ASSERT( svg_num_stops(svg) == 2 );

  svg_destroy(svg);
}

extern void test_svg_explicit(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = build_svg()) != NULL);
  CU_ASSERT( svg_num_stops(svg) == 2 );
  CU_ASSERT( svg_explicit(svg)  == 0 );
  CU_ASSERT( svg_num_stops(svg) == 4 );
  CU_ASSERT( svg_explicit(svg)  == 0 );
  CU_ASSERT( svg_num_stops(svg) == 4 );

  svg_destroy(svg);
}

extern void test_svg_interpolate(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = build_svg()) != NULL);

  rgb_t rgb, rgb_expected = {128, 128, 0};
  double op, op_expected = 0.5;

  CU_ASSERT( svg_interpolate(svg, 0.5, &rgb, &op) == 0 );
  CU_ASSERT( rgb_equal(rgb, rgb_expected) );
  CU_ASSERT( op == op_expected );

  CU_ASSERT( svg_interpolate(svg, 0.0, &rgb, &op) != 0 );

  svg_destroy(svg);
}

extern void test_svg_flatten(void)
{
  svg_t *svg;

  CU_TEST_FATAL( (svg = build_svg()) != NULL);

  rgb_t rgb_bg = {0, 0, 0};

  CU_ASSERT( svg_flatten(svg, rgb_bg) == 0 );

  rgb_t rgb, rgb_expected = {64, 64, 0};
  double op, op_expected = 1.0;

  CU_ASSERT( svg_interpolate(svg, 0.5, &rgb, &op) == 0 );
  CU_ASSERT( rgb_equal(rgb, rgb_expected) );
  CU_ASSERT( op == op_expected );

  svg_destroy(svg);
}
