/*
  ident.c

  identitifiers

  (c) J. J. Green 2002
*/

#include <stdlib.h>
#include <string.h>

#include "ident.h"

extern ident_t* ident_new(char* name,int id)
{
  char* dup;
  ident_t* ident;

  if ((ident = malloc(sizeof(ident_t))) == NULL)
    return NULL;

  if ((dup = strdup(name)) == NULL)
    return NULL;

  ident->name = dup;
  ident->id   = id;

  return ident;
}

extern void ident_destroy(ident_t* ident)
{
  free(ident->name);
  free(ident);
}
  
