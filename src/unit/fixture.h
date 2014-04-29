/*
  fixture.h
  Copyright (c) J.J. Green 2014
*/

#ifndef FIXTURE_H
#define FIXTURE_H

/*
  writes the fixture path to buff, of size n
  returns as snprintf
*/

#include <stdlib.h>

extern int fixture(char*, size_t, const char*, const char*);

#endif
