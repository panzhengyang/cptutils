/*
  odb_bridge.c

  handles the globals needed by odb flex/bison scanner

  $Id: bridge.c,v 1.1 2004/03/18 02:27:54 jjg Exp $
*/

#include <stdio.h>
#include "odb_bridge.h"

odb_t *odb = NULL;
identtab_t *identtab = NULL, *stringtab = NULL;

