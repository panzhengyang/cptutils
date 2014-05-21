/*
  tests_saowrite.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_SAOWRITE_H
#define TESTS_SAOWRITE_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_saowrite[];

extern void test_saowrite_write(void);
extern void test_saowrite_nosuchdir(void);

#endif
