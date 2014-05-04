/*
  tests-cpt.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_CPT_H
#define TESTS_CPT_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_cpt[]; 
extern void test_cpt_new(void);
extern void test_cpt_seg_new(void);
extern void test_cpt_append(void);
extern void test_cpt_pop(void);
extern void test_cpt_shift(void);
extern void test_cpt_zrange(void);
extern void test_cpt_zfill_grey(void);
extern void test_cpt_zfill_rgb(void);

#endif
