/*
  tests_svg.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_SVG_H
#define TESTS_SVG_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_svg[]; 

extern void test_svg_new(void);
extern void test_svg_append(void);
extern void test_svg_prepend(void);
extern void test_svg_each_stop(void);
extern void test_svg_num_stops(void);
extern void test_svg_explicit(void);
extern void test_svg_interpolate(void);
extern void test_svg_flatten(void);

#endif
