/*
  tests_pov.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_POV_H
#define TESTS_POV_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_pov[]; 

extern void test_pov_create(void);
extern void test_pov_set_name_simple(void);
extern void test_pov_set_name_complex(void);
extern void test_pov_alloc_stops(void);

#endif
