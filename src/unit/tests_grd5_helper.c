#include <stdlib.h>
#include <string.h>

#include "tests_grd5_helper.h"

extern grd5_string_t* build_grd5string(const char* content)
{
  grd5_string_t *str;

  if ((str = malloc(sizeof(grd5_string_t))) == NULL)
    return NULL;

  str->len = strlen(content);

  if ((str->content = malloc(str->len)) == NULL)
    return NULL;

  strncpy(str->content, content, str->len);

  return str;
}
