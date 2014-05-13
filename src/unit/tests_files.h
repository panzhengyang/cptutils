/*
  tests_files.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_FILES_H
#define TESTS_FILES_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_files[]; 
extern void test_files_is_readable(void);
extern void test_files_is_absolute_filename(void);

#endif
