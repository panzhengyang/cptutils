/*
  odbbridge.c

  handles the globals needed by odb flex/bison scanner

  $Id: obdbridge.c,v 1.1 2005/11/16 00:28:21 jjg Exp jjg $
*/

#include <stdio.h>
#include "odbbridge.h"

odb_t *odb = NULL;
identtab_t *identtab = NULL, *stringtab = NULL;

