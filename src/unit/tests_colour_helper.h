/*
  tests_colour_helper.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_COLOUR_HELPER_H
#define TESTS_COLOUR_HELPER_H

#include <stdbool.h>
#include <colour.h>

extern bool rgb_equal(rgb_t, rgb_t);
extern bool hsv_equal(hsv_t, hsv_t, double);
extern bool triple_equal(double*, double*, double);

extern rgb_t build_rgb(int, int, int);
extern hsv_t build_hsv(double, double, double);

#endif
