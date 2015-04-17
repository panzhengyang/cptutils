/*
  tests_colour.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_COLOUR_H
#define TESTS_COLOUR_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_colour[]; 

extern void test_colour_hsvD_to_rgbD(void);
extern void test_colour_rgbD_to_hsvD(void);

extern void test_colour_rgbD_to_rgb(void);
extern void test_colour_rgb_to_rgbD(void);

extern void test_colour_rgbD_to_hsv(void);
extern void test_colour_hsv_to_rgbD(void);

extern void test_colour_rgbD_to_hsvD(void);
extern void test_colour_hsvD_to_rgbD(void);

extern void test_colour_hsv_to_rgb(void);

extern void test_colour_grey_to_rgbD(void);

extern void test_colour_rgb_interpolate(void);
extern void test_colour_hsv_interpolate(void);

extern void test_colour_parse_rgb(void);

extern void test_colour_model_name(void);



#endif
