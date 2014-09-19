/*
  btrace.h
  Copyright (c) J.J. Green 2014
*/

#ifndef BTRACE_H
#define BTRACE_H

#include <stdio.h>

extern void btrace_init(void);
extern void btrace_free(void);
extern void btrace_print_plain(FILE*);
extern void btrace_print_json(FILE*);
extern void btrace_add_real(const char* file, int line, const char*, ...);

#define btrace_add(...) btrace_add_real(__FILE__, __LINE__, __VA_ARGS__)

#endif
