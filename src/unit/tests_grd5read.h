/*
  tests_grd5read.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_GRD5READ_H
#define TESTS_GRD5READ_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_grd5read[]; 

extern void test_grd5read_fixtures(void);
extern void test_grd5read_grd3(void);
extern void test_grd5read_nofile(void);

#endif
