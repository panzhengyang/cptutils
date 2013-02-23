 /*
  identtab.c

  a simple identifier table

  (c) J. J. Green 2001
*/

/*
  we use tdestroy(), which is a gnu extension
*/

#define _GNU_SOURCE
#include <search.h>

#include <stdlib.h>
#include <string.h>

#include "identtab.h"

struct identtab_t
{
  void **names,**ids;
};

/*
  new
*/

static void** root_new(void)
{
  void ** root;

  if ((root = malloc(sizeof(void*))) == NULL)
    return NULL;

  *root = NULL;

  return root;
}

extern identtab_t* identtab_new(void)
{
  identtab_t* tab;

  if ((tab = malloc(sizeof(identtab_t))) == NULL ||
      (tab->names = root_new()) == NULL ||
      (tab->ids = root_new()) == NULL)
    return NULL;

  return tab;
}

/* 
   destroy
*/

static void noop(void* arg)
{
  /* do nothing */
}

static void tree_destroy(void** root)
{
  tdestroy(*root,noop);
  free(root);
}

extern void identtab_destroy(identtab_t* tab)
{
  tree_destroy(tab->names);
  tree_destroy(tab->ids);
  
  free(tab);
}

/*
  insert
*/

typedef int (*cmp_t)(const void*,const void*);
typedef int (*scmp_t)(ident_t*,ident_t*);

static int ui_cmp(unsigned int a,unsigned int b)
{
  if (a<b) return -1;
  else
    if (a>b) return 1;
    else return 0;
}

static int ident_id_cmp(ident_t* a,ident_t* b)
{
  return ui_cmp(a->id,b->id);
}

static int ident_name_cmp(ident_t* a,ident_t* b)
{
  return strcmp(a->name,b->name);
}

static int tree_insert(void** root,ident_t* ident,scmp_t cmp)
{
  return (tsearch(ident,root,(cmp_t)cmp) == NULL ? 1 : 0);
}

extern int identtab_insert(identtab_t* tab,ident_t* ident)
{
  return 
    tree_insert(tab->names,ident,ident_name_cmp) ||
    tree_insert(tab->ids,ident,ident_id_cmp);
}

/*
  lookup, note that tfind() returns a pointer to ident_t*
*/

static int ident_name_fcmp(const char* a,ident_t* b)
{
  return strcmp(a,b->name);
}

static int ident_id_fcmp(unsigned int* a,ident_t* b)
{
  return ui_cmp(*a,b->id);
}
   
static ident_t* identtab_lookup(const void* key,void** root,cmp_t cmp)
{
  ident_t ** val;

  val = tfind(key,root,cmp);

  return (val ? *val : NULL);
}  

extern ident_t* identtab_id_lookup(identtab_t* tab,unsigned int id)
{
  return identtab_lookup(&id,tab->ids,(cmp_t)ident_id_fcmp);
}  

extern ident_t* identtab_name_lookup(identtab_t* tab,const char* name)
{
  return identtab_lookup(name,tab->names,(cmp_t)ident_name_fcmp);
}  

/*
  map over elements of the table. We use the twalk() function
  from <search.h>, which means that we have to use a file static
  variable to pass the map an argument.

  the return value is the sum of return values of the mapped
  functions
*/

static void    *identtab_map_opt;
static stmap_t  identtab_map_fun;
static int      identtab_map_rtn;

static void action(const void* node,const VISIT which,const int depth)
{
  switch (which)
    {
    case preorder :
    case endorder: 
      break;
    case postorder :
    case leaf:
      /* the first field of node is the ident_t* */
      identtab_map_rtn += identtab_map_fun(*(ident_t**)node,identtab_map_opt);
      break;
    }
}

static int identtab_map(void** root,stmap_t f,void* opt)
{
  identtab_map_fun = f;
  identtab_map_opt = opt;
  identtab_map_rtn = 0;

  twalk(*root,action);

  return identtab_map_rtn;
}

extern int identtab_map_ids(identtab_t* tab,stmap_t f,void* opt)
{
  return identtab_map(tab->ids,f,opt);
}

extern int identtab_map_names(identtab_t* tab,stmap_t f,void* opt)
{
  return identtab_map(tab->names,f,opt);
}

/*
  number of entries in a ident table
*/

static int stcount(ident_t* ident,void* dummy)
{
  return 1;
}

extern int identtab_size(identtab_t* tab)
{
  return identtab_map(tab->names,(stmap_t)stcount,NULL);
}

/*
  mainly for debugging
*/

static int identprint(ident_t* ident,char* fmt)
{
  printf(fmt,ident->name,ident->id);
  return 0;
}

extern int identtab_printf(identtab_t* tab)
{
  identtab_map_ids(tab,(stmap_t)identprint,"%s <- %i\n");
  identtab_map_names(tab,(stmap_t)identprint,"%s -> %i\n");

  return 0;
}
  

/*

extern int       identtab_ids_dump(identtab_t*,unsigned int*);
*/
