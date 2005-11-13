/*
  avl.c

  avl structures
  J.J. Green 2005
  $Id$
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

/* utility defines */

#define LBUF 1024
#define SNM(x) ((x) ? (x) : "<stdin>")

extern int avl_read(FILE* st,avl_grad_t* avl,int debug)
{
  yyscan_t odbscan;
  odb_t   *odb;

  /* 
     assign global odb* acted upon by the bison parser, I'd like
     this to be an argument for odbparse, but bison only allows 
     one argument & that is used for the scanner
  */

  odb_bridge = odb;

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

  /* read the avl from the odb structure */

  return 0;
}

extern int avl_clean(avl_grad_t* avl)
{
  return 0;
}
