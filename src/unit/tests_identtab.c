/*
  tests_identtab.c
  Copyright (c) J.J. Green 2014
*/

#include <string.h>
#include <identtab.h>
#include "tests_identtab.h"
#include "tests_identtab_helper.h"

CU_TestInfo tests_identtab[] = 
  {    
    { "create",           test_identtab_create },
    { "insert single",    test_identtab_insert_single },
    { "insert duplicate", test_identtab_insert_duplicate },
    { "size",             test_identtab_size },
    { "lookup name",      test_identtab_name_lookup },
    { "lookup id",        test_identtab_id_lookup },
    { "map names",        test_identtab_map_names },
    { "map ids",          test_identtab_map_ids },
    CU_TEST_INFO_NULL,
  };

extern void test_identtab_create(void)
{
  identtab_t *tab;

  CU_TEST_FATAL( (tab = identtab_new()) != NULL );

  identtab_destroy(tab);
}

extern void test_identtab_insert_single(void)
{
  identtab_t *tab;
  CU_TEST_FATAL( (tab = identtab_new()) != NULL );

  ident_t *ident;
  CU_TEST_FATAL( (ident = ident_new("foo", 7)) != NULL );

  CU_ASSERT( identtab_size(tab) == 0 );
  CU_ASSERT( identtab_insert(tab, ident) == 0 );
  CU_ASSERT( identtab_size(tab) == 1 );

  identtab_destroy(tab);
}

/* duplicate names are not inserted */

extern void test_identtab_insert_duplicate(void)
{
  identtab_t *tab;
  CU_TEST_FATAL( (tab = identtab_new()) != NULL );

  ident_t *ident1, *ident2;
  CU_TEST_FATAL( (ident1 = ident_new("foo", 7)) != NULL );
  CU_TEST_FATAL( (ident2 = ident_new("foo", 4)) != NULL );

  CU_ASSERT( identtab_size(tab) == 0 );
  CU_ASSERT( identtab_insert(tab, ident1) == 0 );
  CU_ASSERT( identtab_insert(tab, ident2) == 0 );
  CU_ASSERT( identtab_size(tab) == 1 );

  identtab_destroy(tab);
}

extern void test_identtab_size(void)
{
  identtab_t *tab;

  CU_TEST_FATAL( (tab = build_identtab(25)) != NULL );
  CU_ASSERT( identtab_size(tab) == 25 );

  identtab_destroy(tab);
}

extern void test_identtab_name_lookup(void)
{
  identtab_t *tab;

  CU_TEST_FATAL( (tab = build_identtab(25)) != NULL );

  CU_ASSERT( identtab_name_lookup(tab, "not-there") == NULL );

  ident_t *ident;

  CU_TEST_FATAL( (ident = identtab_name_lookup(tab, "id-20")) != NULL );
  CU_ASSERT( ident->id == 20 );

  identtab_destroy(tab);
}

extern void test_identtab_id_lookup(void)
{
  identtab_t *tab;

  CU_TEST_FATAL( (tab = build_identtab(25)) != NULL );

  ident_t *ident;

  CU_TEST_FATAL( (ident = identtab_id_lookup(tab, 19)) != NULL );
  CU_ASSERT( strcmp(ident->name, "id-19") == 0 );

  identtab_destroy(tab);
}

static int sum_ids_cb(ident_t *ident, int *sum)
{
  *sum += ident->id;
  return 0;
}

extern void test_identtab_map_names(void)
{
  identtab_t *tab;
  int sum = 0;

  CU_TEST_FATAL( (tab = build_identtab(5)) != NULL );
  CU_ASSERT( identtab_map_names(tab, (stmap_t)sum_ids_cb, (void*)&sum) == 0 );
  CU_ASSERT( sum == 10 );

  identtab_destroy(tab);
}

extern void test_identtab_map_ids(void)
{
  identtab_t *tab;
  int sum = 0;

  CU_TEST_FATAL( (tab = build_identtab(5)) != NULL );
  CU_ASSERT( identtab_map_ids(tab, (stmap_t)sum_ids_cb, (void*)&sum) == 0 );
  CU_ASSERT( sum == 10 );

  identtab_destroy(tab);
}

