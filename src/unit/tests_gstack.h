/*
  tests_gstack.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_GSTACK_H
#define TESTS_GSTACK_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_gstack[]; 

extern void test_gstack_new(void);
extern void test_gstack_pushpop(void);
extern void test_gstack_reverse(void);
extern void test_gstack_foreach(void);

#endif
