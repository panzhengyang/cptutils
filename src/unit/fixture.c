#include <stdio.h>
#include "fixture.h"

#define FIXTURE_BASE "../t"

extern int fixture(char *buff, size_t n, const char *type, const char *file)
{
  return snprintf(buff, n, "%s/%s/%s", FIXTURE_BASE, type, file);
}
