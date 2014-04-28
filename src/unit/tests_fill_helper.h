/*
  tests_fill_helper.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_FILL_HELPER_H
#define TESTS_FILL_HELPER_H

#include <fill.h>

extern fill_t build_fill_grey(int g);
extern fill_t build_fill_rgb(int r, int g, int b);
extern fill_t build_fill_hsv(double h, double s, double v);

#endif
