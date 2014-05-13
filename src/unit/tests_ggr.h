/*
  tests_ggr.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_GGR_H
#define TESTS_GGR_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_ggr[]; 

extern void test_ggr_load(void);
extern void test_ggr_save(void);
extern void test_ggr_sample(void);

#endif
