/*
  tests_colour.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_BTRACE_H
#define TESTS_BTRACE_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_btrace[]; 

extern void test_btrace_default(void);
extern void test_btrace_enable(void);
extern void test_btrace_disable(void);
extern void test_btrace_count(void);
extern void test_btrace_add(void);
extern void test_btrace_print_plain(void);

#endif
