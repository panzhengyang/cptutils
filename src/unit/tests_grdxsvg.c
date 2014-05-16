/*
  tests_grdxsvg.h
  Copyright (c) J.J. Green 2014
*/

#include <grdxsvg.h>
#include "tests_grdxsvg.h"

CU_TestInfo tests_grdxsvg[] = 
  {
    {"2-segment conversion", test_grdxsvg_2seg},
    CU_TEST_INFO_NULL,
  };

extern void test_grdxsvg_2seg(void)
{
  size_t i;

  rgb_stop_t rgb_stops[] =
    {
      {4096, 1.0, 0.50, 0.0},
      {2048, 0.5, 0.25, 0.0},
      {0,    0.0, 0.00, 0.0}
    };
  size_t nrgb = sizeof(rgb_stops)/sizeof(rgb_stop_t);
  gstack_t *rgb_stack = gstack_new(sizeof(rgb_stop_t), nrgb, nrgb); 

  CU_TEST_FATAL( rgb_stack != NULL );

  for (i=0 ; i<nrgb ; i++)
    CU_TEST_FATAL( gstack_push(rgb_stack, rgb_stops + i) == 0 );

  op_stop_t op_stops[] =
    {
      {4096, 0.5},
      {0,    0.5}
    };
  size_t nop = sizeof(op_stops)/sizeof(op_stop_t);
  gstack_t *op_stack = gstack_new(sizeof(op_stop_t), nop, nop); 

  CU_TEST_FATAL( op_stack != NULL );

  for (i=0 ; i<nop ; i++)
    CU_TEST_FATAL( gstack_push(op_stack, op_stops + i) == 0 );

  svg_t *svg = svg_new();

  CU_TEST_FATAL( svg != NULL );

  CU_ASSERT( grdxsvg(rgb_stack, op_stack, svg) == 0 );
  CU_ASSERT( svg_num_stops(svg) == 3 );

  svg_destroy(svg);

  gstack_destroy(op_stack);
  gstack_destroy(rgb_stack);
}
