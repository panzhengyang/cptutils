/*
  tests_btrace.h
  Copyright (c) J.J. Green 2014
*/

#include <unistd.h>
#include <btrace.h>
#include "tests_btrace.h"

CU_TestInfo tests_btrace[] = 
  {
    {"default",       test_btrace_default},
    {"enable",        test_btrace_enable},
    {"disable",       test_btrace_disable},
    {"test empty",    test_btrace_is_empty},
    {"format",        test_btrace_format},
    {"count",         test_btrace_count},
    {"add",           test_btrace_add},
    {"print to file", test_btrace_print},
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
  btrace_enable("foo");
  CU_ASSERT( btrace_is_enabled() == true );
  btrace_disable();
  CU_TEST_FATAL( btrace_is_enabled() == false );
}

/* btrace_disable() should change the enabled state */

extern void test_btrace_disable(void)
{
  btrace_enable("foo");
  CU_TEST_FATAL( btrace_is_enabled() == true );
  btrace_disable();
  CU_ASSERT( btrace_is_enabled() == false );
}

/* btrace_is_empty should initally be false */

extern void test_btrace_is_empty(void)
{
  CU_ASSERT( btrace_is_empty() == true );
  btrace_enable("foo");
  btrace("a message");
  CU_ASSERT( btrace_is_empty() == false );
  btrace_reset();
  CU_ASSERT( btrace_is_empty() == true );
  btrace_disable();
}

/* btrace_count() should return 0 by default */

extern void test_btrace_count(void)
{
  CU_ASSERT( btrace_count() == 0 );
  btrace_enable("foo");
  btrace("a message");
  CU_ASSERT( btrace_count() == 1 );
  btrace_reset();
  CU_ASSERT( btrace_count() == 0 );
  btrace_disable();
}

/* get (integer) format from string */

extern void test_btrace_format(void)
{
  CU_ASSERT( btrace_format(NULL)    == BTRACE_NONE  );
  CU_ASSERT( btrace_format("plain") == BTRACE_PLAIN );
  CU_ASSERT( btrace_format("xml")   == BTRACE_XML   );
  CU_ASSERT( btrace_format("json")  == BTRACE_JSON  );
  CU_ASSERT( btrace_format("goats") == BTRACE_ERROR );
}

/* adding lines should increase the count */

extern void test_btrace_add(void)
{
  btrace_enable("foo");
  btrace("no arguments");
  btrace("%d arguments", 1);
  btrace("%d %s", 2, "arguments");
  CU_ASSERT( btrace_count() == 3 );
  btrace_reset();
  btrace_disable();
}

/*
  print_plain should print lines to the specifed FILE 
  and the lines should start with the message added
*/

#define MESSAGE "a message"

/* just a stub at present */

extern void test_btrace_print(void)
{
  char *path;
  CU_TEST_FATAL( (path = tmpnam(NULL)) != NULL );

  btrace_enable("foo");
  btrace(MESSAGE);

  btrace_print(path, BTRACE_XML);
  
  btrace_reset();
  btrace_disable();

  unlink(path);
}

