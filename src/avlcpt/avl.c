/*
  avl.c

  avl structures
  J.J. Green 2005
  $Id: avl.c,v 1.2 2005/11/16 00:27:57 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "avl.h"
#include "odb.h"
#include "odb_bridge.h"
#include "odbparse.h"
#include "odbscan.h"

/* 
   defined in odbparse.c but declared here -
   odbparse() is the main parser, odbdebug the
   parser debug flag
*/

extern int odbparse(void*);
extern int odbdebug;

static int odb_avl(odb_t*,identtab_t*,identtab_t*,avl_grad_t*,int);

/* main avl construction function */

extern int avl_read(FILE* st,avl_grad_t* avl,int verbose,int debug)
{
  yyscan_t odbscan;

  /* 
     assign global identtab acted upon by the bison parser, I'd like
     this to be an argument for odbparse, but bison only allows 
     one argument & that is used for the scanner
  */

  identtab  = identtab_new();
  stringtab = identtab_new();

  if (!(identtab && stringtab))
    {
      fprintf(stderr,"failed to create identifier tables\n");
      return 1;
    }

  /*
    setup scanner
  */

  if (odblex_init(&odbscan) != 0)
    {
      fprintf(stderr,"problem initailising scanner : %s\n",strerror(errno));
      return 1;
    }
  
  odbset_in(st,odbscan);
  odbset_debug(debug,odbscan);

  /*
    do the parse
  */

  odbdebug = debug;

  if (odbparse(odbscan) != 0)
    {
      fprintf(stderr,"failed parse\n");
      return 1;
    }

  odblex_destroy(odbscan);

  /* 
     the parse should have produced an odb_t structure in
     the variable odb (declared & defined in obd_bridge),
     as well as filling the idenntab and string tab, which 
     need to be refered to when interpreting odb_string
     and odb_ident quantities.
  */

  if (odb == NULL)
    {
      fprintf(stderr,"parse successful but abstract syntax tree for ODB not created\n");
      return 1;
    }

  if (verbose)
    {
      printf("input is ODB version %i.%i\n",odb->version.major,odb->version.minor);
      printf("  %i identifiers\n",identtab_size(identtab));
      printf("  %i strings\n",identtab_size(stringtab));
    }

  if (odb_serialise(odb,identtab) != 0)
    {
      fprintf(stderr,"failed serialisation of ODB object\n");
      return 1;
    }

  if (odb_avl(odb,identtab,stringtab,avl,verbose) != 0)
    {
      fprintf(stderr,"failed avl extraction\n");
      return 1;
    }

  /* clean up */

  odb_destroy(odb);

  identtab_destroy(identtab);
  identtab_destroy(stringtab);

  return 0;
}

/*
  extract avl gradient from an odb serialised structure
*/

static int odb_avl(odb_t* odb,identtab_t* itab,identtab_t* stab,avl_grad_t* avl, int verbose)
{
  odb_record_t* r1;
  odb_field_t* f1;

  r1 = odb_class_name_lookup("ODB",itab,odb);
  f1 = odb_attribute_name_lookup("Roots",itab,r1);

  return 0;
}

extern int avl_clean(avl_grad_t* avl)
{
  return 0;
}
