/*
  tests_cptname.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_CPTNAME_H
#define TESTS_CPTNAME_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_cptname[]; 
extern void test_cptname_specific(void);
extern void test_cptname_wildcard(void);
extern void test_cptname_null(void);

#endif
