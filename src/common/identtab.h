/*
  identtab.h 

  symbol table interface

  (c) J. J. Green 2002
  $Id: identtab.h,v 1.1 2002/08/28 21:44:41 jjg Exp $
*/

#ifndef IDENTTAB_H
#define IDENTTAB_H

#include <stdio.h>
#include <search.h>

#include "ident.h"

typedef struct identtab_t identtab_t;

extern identtab_t* identtab_new(void);
extern void identtab_destroy(identtab_t*);

extern int      identtab_insert(identtab_t*,ident_t*);
extern ident_t* identtab_name_lookup(identtab_t*,char*);
extern ident_t* identtab_id_lookup(identtab_t*,unsigned int);
extern int      identtab_printf(identtab_t*);
extern int      identtab_size(identtab_t*);
extern int      identtab_ids_dump(identtab_t*,unsigned int*);

typedef int (*stmap_t)(ident_t*,void*);

extern int identtab_map_ids(identtab_t*,stmap_t,void*);
extern int identtab_map_names(identtab_t*,stmap_t,void*);

#endif
