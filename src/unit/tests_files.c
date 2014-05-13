/*
  tests_files.c
  Copyright (c) J.J. Green 2014
*/

#include <sys/stat.h>
#include <stdio.h>

#include <files.h>
#include "tests_files.h"

CU_TestInfo tests_files[] = 
  {
    {"file is readable", test_files_is_readable},
    {"path is absolute", test_files_is_absolute_filename},
    CU_TEST_INFO_NULL,
  };

extern void test_files_is_readable(void)
{
  char *path = tmpnam(NULL);
  FILE *st = fopen(path, "w");
  fprintf(st, "hello\n");
  fclose(st);

  chmod(path, 0);
  CU_ASSERT( ! is_readable(path) );

  chmod(path, S_IRUSR | S_IWUSR);
  CU_ASSERT( is_readable(path) );

  unlink(path);
  CU_ASSERT(! is_readable(path));
}

extern void test_files_is_absolute_filename(void)
{
  CU_ASSERT( is_absolute_filename("/very/much/so") );
  CU_ASSERT( ! is_absolute_filename("not/at/all") );
}

