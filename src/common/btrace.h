/*
  btrace.h
  Copyright (c) J.J. Green 2014
*/

#ifndef BTRACE_H
#define BTRACE_H

#include <stdio.h>
#include <stdbool.h>

extern void btrace_enable(void);
extern void btrace_disable(void);
extern bool btrace_is_enabled(void);

extern void   btrace_reset(void);
extern size_t btrace_count(void);
extern void   btrace_print_plain(FILE*);
extern void   btrace_print_xml(FILE*);
extern void   btrace_add_real(const char* file, int line, const char*, ...);

#define btrace_add(...) btrace_add_real(__FILE__, __LINE__, __VA_ARGS__)

#endif
