/*
  tests_identtab_helper.c
  Copyright (c) J.J. Green 2014
*/

#include "tests_identtab_helper.h"

extern identtab_t* build_identtab(int n)
{
  identtab_t *tab;

  if ( (tab = identtab_new()) == NULL )
    return NULL;

  int i;

  for (i=0 ; i<n ; i++)
    {
      ident_t *ident;
      char buf[36];

      sprintf(buf, "id-%02i", i);
      if ( (ident = ident_new(buf, i)) == NULL )
	return NULL;

      if ( identtab_insert(tab, ident) != 0 )
	return NULL;
    }

  return tab;
}


