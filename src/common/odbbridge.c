/*
  odbbridge.c

  handles the globals needed by odb flex/bison scanner

  $Id: obdbridge.c,v 1.2 2012/03/09 21:43:01 jjg Exp $
*/

#include <stdio.h>
#include "odbbridge.h"

odb_t *odb = NULL;
identtab_t *identtab = NULL, *stringtab = NULL;

