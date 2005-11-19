/*
  avl.c

  avl structures
  J.J. Green 2005
  $Id: avl.c,v 1.4 2005/11/18 00:27:16 jjg Exp jjg $
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
  odb_record_t *Rl,*Rs;
  odb_uint_t odbid,legid,symid,*idxs,*idxl;
  avl_seg_t* segs = NULL; 
  int i,n,ns,nl,class,colour,child;

  /* get Legend and SymTab records */

  if ((odbid = lookup_id("ODB",itab)) == 0 ||
      traverse(odbid,"Roots",itab,odb,&legid) != 0  ||
      traverse(legid,"Symbols",itab,odb,&symid) != 0)
    return 1;

  if ((Rl = odb_class_id_lookup(legid,odb)) == NULL)
    {
      fprintf(stderr,"failed to find Legend record\n");
      return 1;
    }

  if ((Rs = odb_class_id_lookup(symid,odb)) == NULL)
    {
      fprintf(stderr,"failed to find SymList record\n");
      return 1;
    }

  /*
    the label strings are held in the class attribute, while 
    the colours are in the symtab. To merge these we first create 
    2 arrays which index the appropriate fields
  */

  class  = lookup_id("Class",itab);
  child  = lookup_id("Child",itab);
  colour = lookup_id("Color",itab);

  /* count legends & colors, plus some sanity checking */

  if ((n = Rl->n) > 0)
    {
      odb_field_t* fs;

      fs = Rl->fields;

      for (i=0,nl=0 ; i<n ; i++)
	{
	  if (fs[i].attribute == class) nl++;
	}

      if (!nl)
	{
	  fprintf(stderr,"no Class field in Legend record\n");
	  return 1;
	}
    }
  else
    {
      fprintf(stderr,"Legend record is empty\n");
      return 1;
    }

  if ((n = Rs->n) > 0)
    {
      odb_field_t* fs;

      fs = Rs->fields;

      for (i=0,ns=0 ; i<n ; i++)
	{
	  if (fs[i].attribute == child) ns++;
	}

      if (!ns)
	{
	  fprintf(stderr,"no Child field in SymTab record\n");
	  return 1;
	}
    }
  else
    {
      fprintf(stderr,"SymTab record is empty\n");
      return 1;
    }

  if (verbose)
    {
      printf("found %i/%i legend, %i/%i colour\n",nl,Rl->n,ns,Rs->n);
    }

  if (ns != nl)
    {
      fprintf(stderr,"unbalanced legend : %i legends, %i colours\n",nl,ns);
      return 1;
    }

  /* create indicies */

  idxs = malloc(ns*sizeof(int));
  idxl = malloc(nl*sizeof(int));

  if ((!idxs) || (!idxl))
    {
      fprintf(stderr,"out of memory\n");
      return 1;
    }

  if ((n = Rl->n) > 0)
    {
      int i,m;
      odb_field_t* fl = Rl->fields;

      for (i=0,m=0 ; i<n ; i++)
	{
	  if (fl[i].attribute == class)
	    {
	      idxl[m] = i;
	      m++;
	    }
	}
    }

  if ((n = Rs->n) > 0)
    {
      int i,m;
      odb_field_t* fs = Rs->fields;

      for (i=0,m=0 ; i<n ; i++)
	{
	  if (fs[i].attribute == child)
	    {
	      idxs[m] = i;
	      m++;
	    }
	}
    }

  /* segments */

  if ((segs = malloc(ns*sizeof(avl_seg_t))) == NULL)
    {
      fprintf(stderr,"out of memory\n");
      return 1;
    }

  for (i=0 ; i<nl ; i++)
    {
      odb_uint_t lcid,colid,bshid;
      odb_record_t *rl,*rs;
      int labid,il,is;
      odb_field_t *fl,*fs,*fr,*fg,*fb;
      avl_seg_t seg;      
      const char* label;

      /* get legend string */

      il = idxl[i];
      fl = Rl->fields + il;
      
      if (fl->attribute != class)
	{
	  fprintf(stderr,"indexing error\n");
	  return 1;
	}
      
      if (fl->type != odb_uint)
	{
	  fprintf(stderr,"Class %i value not an integer type!\n",il);
	  return 1;
	}
      
      lcid = fl->value.u;
      
      if ((rl = odb_class_id_lookup(lcid,odb)) == NULL)
	{
	  fprintf(stderr,"failed to find LClass record\n");
	  return 1;
	}

      if ((fl = odb_attribute_name_lookup("Label",itab,rl)) == NULL)
	{
	  fprintf(stderr,"failed to find Label field in LClass\n");
	  return 1;
	}

      if (fl->type != odb_string)
	{
	  fprintf(stderr,"Label value not a string type!\n");
	  return 1;
	}
      
      labid = fl->value.s;

      if ((label = lookup_name(labid,stab)) == NULL) return 1;

      /* get the colours */

      is = idxs[i];
      fs = Rs->fields + is;
      
      if (fs->attribute != child)
	{
	  fprintf(stderr,"indexing error\n");
	  return 1;
	}
      
      if (fs->type != odb_uint)
	{
	  fprintf(stderr,"Color %i value not an integer type!\n",is);
	  return 1;
	}
      
      bshid = fs->value.u;
      
      if ((rs = odb_class_id_lookup(bshid,odb)) == NULL)
	{
	  fprintf(stderr,"failed to find BshSym record\n");
	  return 1;
	}

      if ((fs = odb_attribute_ident_lookup(colour,rs)) == NULL)
	{
	  fprintf(stderr,"failed to find Color field in BBhSym\n");
	  return 1;
	}

      if (fs->type != odb_uint)
	{
	  fprintf(stderr,"Color %i value not an integer type!\n",is);
	  return 1;
	}
      
      colid = fs->value.u;
            
      if ((rs = odb_class_id_lookup(colid,odb)) == NULL)
	{
	  fprintf(stderr,"failed to find Color record\n");
	  return 1;
	}

      fr = odb_attribute_name_lookup("Red",itab,rs);
      fg = odb_attribute_name_lookup("Green",itab,rs);
      fb = odb_attribute_name_lookup("Blue",itab,rs);
      
      if (fr && fg && fb)
	{
	  if (fr->type != odb_hex4 ||
	      fg->type != odb_hex4 ||
	      fb->type != odb_hex4)
	    {
	      fprintf(stderr,"Color component not a hex4 type!\n");
	      return 1;
	    }

	  /* now we have everthing we need so we write to the segment */
	  
	  seg.nodata = 0;
	  seg.r      = fr->value.h4;
	  seg.g      = fg->value.h4;
	  seg.b      = fb->value.h4;
	  seg.label  = strdup(label);
	}
      else
	{
	  /* probably a nodata legend, mark as such */
	
	  seg.nodata = 1;
	  seg.r      = 0;
	  seg.g      = 0;
	  seg.b      = 0;
	  seg.label  = NULL;
	}

      segs[i] = seg;
    }

  avl->n   = ns;
  avl->seg = segs;

  return 0;
}

extern int avl_clean(avl_grad_t* avl)
{
  /* FIXME */

  return 0;
}
