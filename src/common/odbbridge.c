/*
  odbbridge.c

  handles the globals needed by odb flex/bison scanner
*/

#include <stdio.h>
#include "odbbridge.h"

odb_t *odb = NULL;
identtab_t *identtab = NULL, *stringtab = NULL;

