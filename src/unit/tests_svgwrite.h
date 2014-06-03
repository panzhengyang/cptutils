/*
  tests_svgwrite.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_SVGWRITE_H
#define TESTS_SVGWRITE_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_svgwrite[]; 

extern void test_svgwrite_fixtures(void);
extern void test_svgwrite_nosuchdir(void);

#endif
