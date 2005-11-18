/*
  avl.c

  avl structures
  J.J. Green 2005
  $Id: avl.c,v 1.3 2005/11/17 00:01:44 jjg Exp jjg $
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

static int lookup_id(const char* name,identtab_t* tab)
{
  ident_t* ident;

  if ((ident = identtab_name_lookup(tab,name)) == NULL)
    {
      fprintf(stderr,"failed lookup for %s\n",name);
      return 0;
    }

  return ident->id;
}

static const char* lookup_name(int id,identtab_t* tab)
{
  ident_t* ident;

  if ((ident = identtab_id_lookup(tab,id)) == NULL)
    {
      fprintf(stderr,"failed lookup for %i\n",id);
      return 0;
    }
  
  return ident->name;
}

static int traverse(odb_uint_t id,const char* att,identtab_t* tab,odb_t* odb,odb_uint_t* pid)
{
  odb_record_t* r;
  odb_field_t* f;

  if ((r = odb_class_id_lookup(id,odb)) == NULL)
    {
      fprintf(stderr,"failed to find record (id %i)\n",id);
      return 1;
    }

  if ((f = odb_attribute_name_lookup(att,tab,r)) == NULL)
    {
      fprintf(stderr,"failed to find field with attribute %s\n",att);
      return 1;
    }

  if (f->type != odb_uint)
    {
      fprintf(stderr,"value of attribute %s not an integer type!\n",att);
      return 1;
    }

  *pid = f->value.u;

  return 0;
}

static int odb_avl(odb_t* odb,identtab_t* itab,identtab_t* stab,avl_grad_t* avl, int verbose)
{
  odb_record_t *rl,*rs;
  odb_uint_t odbid,legid,symid;
  avl_seg_t* seg = NULL; 
  int n;

  odbid = lookup_id("ODB",itab);
  printf("ODB\t%u\n",odbid);

  traverse(odbid,"Roots",itab,odb,&legid);
  printf("Legend\t%u\n",legid);

  traverse(legid,"Symbols",itab,odb,&symid);
  printf("SymTab\t%u\n",symid);

  if ((rl = odb_class_id_lookup(legid,odb)) == NULL)
    {
      fprintf(stderr,"failed to find SymList record\n");
      return 1;
    }

  if ((rs = odb_class_id_lookup(symid,odb)) == NULL)
    {
      fprintf(stderr,"failed to find SymList record\n");
      return 1;
    }

  if ((n = rl->n) > 0)
    {
      int cid,i,j,m;
      odb_field_t* fs;
      ident_t* ident;

      fs = rl->fields;

      cid = lookup_id("Class",itab);

      for (i=0,m=0 ; i<n ; i++)
	{
	  if (fs[i].attribute == cid) m++;
	}

      if (!m)
	{
	  fprintf(stderr,"no Classes in Legend\n");
	  return 1;
	}

      printf("found %i classes\n",m);

      if ((seg = malloc(m*sizeof(avl_seg_t))) == NULL)
	{
	  fprintf(stderr,"out of memory\n");
	  return 1;
	}

      for (i=0,j=0 ; i<n ; i++)
	{
	  odb_uint_t lcid;
	  odb_record_t *r;
	  odb_field_t* f;
	  int label;

	  f = fs + i;

	  if (f->attribute != cid) continue;

	  if (f->type != odb_uint)
	    {
	      fprintf(stderr,"Child %i value not an integer type!\n",j+1);
	      return 1;
	    }

	  lcid = f->value.u;

	  // printf("LClass\t%u\n",lcid);

	  if ((r = odb_class_id_lookup(lcid,odb)) == NULL)
	    {
	      fprintf(stderr,"failed to find LClass record\n");
	      return 1;
	    }

	  if ((f = odb_attribute_name_lookup("Label",itab,r)) == NULL)
	    {
	      fprintf(stderr,"failed to find Label field in LClass\n");
	      return 1;
	    }

	  if (f->type != odb_string)
	    {
	      fprintf(stderr,"Label value not a string type!\n");
	      return 1;
	    }

	  label = f->value.s;

	  printf("  %s\n",lookup_name(label,stab));

	  /*
	  odb_field_t *fr,*fg,*fb;

	  if ((r = odb_class_id_lookup(id,odb)) == NULL)
	    {
	      fprintf(stderr,"failed to find Color record\n");
	      return 1;
	    }

	  fr = odb_attribute_name_lookup("Red",itab,r);
	  fg = odb_attribute_name_lookup("Green",itab,r);
	  fb = odb_attribute_name_lookup("Blue",itab,r);

	  if ( ! (fr && fg && fb))
	    {
	      fprintf(stderr,"failed to find components of Color %p %p %p\n",fr,fg,fb);
	      return 1;
	    }

	  if ((fr->type != odb_hex4) )
	    {
	      fprintf(stderr,"Color component not a hex4 type!\n");
	      return 1;
	    }
	  */

	}
    }

  return 0;
}

extern int avl_clean(avl_grad_t* avl)
{
  return 0;
}
