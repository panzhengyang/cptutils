/*
  tests_sao.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_SAO_H
#define TESTS_SAO_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_sao[]; 

extern void test_sao_create(void);
extern void test_sao_push(void);
extern void test_sao_each(void);

#endif
