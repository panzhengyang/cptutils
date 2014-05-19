/*
  tests_identtab.h
  Copyright (c) J.J. Green 2014
*/

#ifndef TESTS_IDENTTAB_H
#define TESTS_IDENTTAB_H

#include <CUnit/CUnit.h>

extern CU_TestInfo tests_identtab[]; 

extern void test_identtab_create(void);
extern void test_identtab_insert_single(void);
extern void test_identtab_insert_duplicate(void);
extern void test_identtab_size(void);
extern void test_identtab_name_lookup(void);
extern void test_identtab_id_lookup(void);
extern void test_identtab_map_ids(void);
extern void test_identtab_map_names(void);

#endif
