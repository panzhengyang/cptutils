/*
  tests_svglist.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_SVGLIST_H
#define TESTS_SVGLIST_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_svglist[]; 

extern void test_svglist_new(void);
extern void test_svglist_svg(void);
extern void test_svglist_iterate(void);
extern void test_svglist_select(void);

#endif
