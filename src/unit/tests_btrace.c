/*
  tests_btrace.h
  Copyright (c) J.J. Green 2014
*/

#include <unistd.h>
#include <btrace.h>
#include "tests_btrace.h"

CU_TestInfo tests_btrace[] = 
  {
    {"default",     test_btrace_default},
    {"enable",      test_btrace_enable},
    {"disable",     test_btrace_disable},
    {"count",       test_btrace_count},
    {"add",         test_btrace_add},
    {"print plain", test_btrace_print_plain},
    CU_TEST_INFO_NULL,
  };

/* btrace should be disabled by default */

extern void test_btrace_default(void)
{
  CU_ASSERT( btrace_is_enabled() == false );
}

/* btrace_enable() should change the enabled state */

extern void test_btrace_enable(void)
{
  CU_TEST_FATAL( btrace_is_enabled() == false );
  btrace_enable();
  CU_ASSERT( btrace_is_enabled() == true );
  btrace_disable();
  CU_TEST_FATAL( btrace_is_enabled() == false );
}

/* btrace_disable() should change the enabled state */

extern void test_btrace_disable(void)
{
  btrace_enable();
  CU_TEST_FATAL( btrace_is_enabled() == true );
  btrace_disable();
  CU_ASSERT( btrace_is_enabled() == false );
}

/* btrace_count() should return 0 by default */

extern void test_btrace_count(void)
{
  CU_ASSERT( btrace_count() == 0 );
  btrace_enable();
  btrace_add("a message");
  CU_ASSERT( btrace_count() == 1 );
  btrace_reset();
  CU_ASSERT( btrace_count() == 0 );
  btrace_disable();
}

/* adding lines should increase the count */

extern void test_btrace_add(void)
{
  btrace_enable();
  btrace_add("no arguments");
  btrace_add("%d arguments", 1);
  btrace_add("%d %s", 2, "arguments");
  CU_ASSERT( btrace_count() == 3 );
  btrace_reset();
  btrace_disable();
}

/*
  print_plain should print lines to the specifed FILE 
  and the lines should start with the message added
*/

#define MESSAGE "a message"

extern void test_btrace_print_plain()
{
  char *path;
  CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL );

  btrace_enable();
  btrace_add(MESSAGE);
  
  FILE *stream;
  CU_TEST_FATAL( (stream = fopen(path, "w")) != NULL );

  btrace_print_plain(stream);

  CU_TEST_FATAL( fclose(stream) == 0 );
  
  btrace_reset();
  btrace_disable();

  size_t msg_size = strlen(MESSAGE);
  char msg[msg_size];

  CU_TEST_FATAL( (stream = fopen(path, "r")) != NULL );
  CU_TEST_FATAL( fread(msg, 1, msg_size, stream) == msg_size );
  CU_TEST_FATAL( fclose(stream) == 0 );
  
  CU_ASSERT( strncmp(msg, MESSAGE, msg_size) == 0 );

  unlink(path);
}
