/*
  tests-cpt-helper.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_CPT_HELPER_H
#define TESTS_CPT_HELPER_H

#include <cpt.h>

extern fill_t build_fill_grey(int g);
extern fill_t build_fill_rgb(int r, int g, int b);
extern cpt_seg_t* build_segment(fill_t f0, fill_t f2, double z0, double z1);

#endif
