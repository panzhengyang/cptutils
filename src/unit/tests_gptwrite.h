/*
  tests_gptwrite.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_GPTWRITE_H
#define TESTS_GPTWRITE_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_gptwrite[]; 

extern void test_gptwrite_write(void);
extern void test_gptwrite_nosuchdir(void);

#endif
