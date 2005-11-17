/*
  odb.c

  various utility functions for creating/destroying
  odb obhjects

  J.J.Green 2005
  $Id: odb.c,v 1.1 2005/11/16 00:27:47 jjg Exp jjg $
*/

#include <stdlib.h>

#include "odb.h"

/* creation of odb record list - used in parser actions */  

extern odb_field_list_t* odb_create_field_list(odb_ident_t att,odb_value_t val)
{
  odb_field_list_t* field;

  if ((field = malloc(sizeof(odb_field_list_t))) == NULL) return NULL;

  field->attribute = att;
  field->value     = val;
  field->next      = NULL;

  return field;
}

extern odb_record_list_t* odb_create_record_list(odb_ident_t class,
						 odb_uint_t id,
						 odb_field_list_t *fields)
{
  odb_record_list_t* record;

  if ((record = malloc(sizeof(odb_record_list_t))) == NULL) return NULL;

  record->class   = class;
  record->id      = id;
  record->fields  = fields;
  record->next    = NULL;

  return record;
}

extern odb_t* odb_create_list(odb_uint_t major,odb_uint_t minor,odb_record_list_t* list)
{
  odb_t* odb;

  if ((odb = malloc(sizeof(odb_t))) == NULL) return NULL;

  odb->list = list;

  odb->n = 0; odb->records = NULL;

  odb->version.major = major;
  odb->version.minor = minor;

  return odb;
}

/* destroy an odb object */

static void odb_destroy_record_list(odb_record_list_t*);

extern void odb_destroy(odb_t* odb)
{
  int n = odb->n;

  if (n)
    {
      int i; 
      odb_record_t* rec = odb->records;

      for (i=0 ; i<n ; i++)
	{
	  if (rec[i].n > 0)
	    {
	      free(rec[i].fields);
	    }
	}

      free(rec);
    }

  if (odb->list)
    {
      odb_destroy_record_list(odb->list);
    }

  free(odb);

  return;
}

/* recursively destroy an odb record-list  */

static void odb_destroy_field_list(odb_field_list_t*);

static void odb_destroy_record_list(odb_record_list_t* rec)
{
  if (rec)
    {
      odb_destroy_record_list(rec->next);
      odb_destroy_field_list(rec->fields);
      free(rec);
    }

  return;
}

static void odb_destroy_field_list(odb_field_list_t* f)
{
  if (f)
    {
      odb_destroy_field_list(f->next);
      free(f);
    }

  return;
}

/*
  here we serialise the odb abstract syntax tree, converting linked
  lists to arrays 
*/

static int record_serialise(odb_record_list_t*,identtab_t*,odb_record_t*);

extern int odb_serialise(odb_t* odb,identtab_t* tab)
{
  odb_record_list_t* recls;
  odb_record_t* rec;
  int n,i;

  if (! odb->list)
    {
      fprintf(stderr,"attempt to serialise ODB without list!\n");
      return 0;
    }

  for (n=0, recls=odb->list ; recls ; recls = recls->next) n++;

  if (!n)
    {
      fprintf(stderr,"ODB has no records\n");
      return 1;
    }

  if ((rec = malloc(n*sizeof(odb_record_t))) == NULL)
    {
      fprintf(stderr,"out of memory\n");
      return 1;
    }

  for (recls = odb->list, i=0 ; recls ; recls = recls->next, i++)
    {
      printf("ser: %i %i %i\n",recls->id,n-1-i,recls->class);

      /* reverse the linked list order */

      if (record_serialise(recls,tab,rec+(n-1-i)) != 0)
	{
	  odb_ident_t class;
	  odb_uint_t  id;
	  ident_t* ident;

	  id    = recls->id;
	  class = recls->class; 
	  
	  if ((ident = identtab_id_lookup(tab,class)) == NULL)
	    fprintf(stderr,"failed to serialise record %i (and class lookup failed)\n",id);
	  else
	    fprintf(stderr,"failed to serialise record %s.%i\n",ident->name,id);

	  return 1;
	}
    }

  odb->n       = n;
  odb->records = rec;

  return 0;
}

static int field_serialise(odb_field_list_t*,odb_field_t*);

static int record_serialise(odb_record_list_t* recls,identtab_t* tab,odb_record_t* rec)
{
  odb_field_list_t* fl;
  odb_field_t* f;
  int n;

  rec->n = 0;
  rec->fields = NULL;

  for (n=0,fl=recls->fields ; fl ; fl=fl->next) n++; 

  if (n)
    {
      int i;

      if ((f = malloc(n*sizeof(odb_field_t))) == NULL)
	{
	  fprintf(stderr,"out of memory\n");
	  return 1;
	}

      for (i=0,fl=recls->fields ; fl ; fl=fl->next,i++)
	{
	  /* reverse the linked list order */

	  if (field_serialise(fl,f+(n-i-1)) != 0)
	    {
	      odb_ident_t att;
	      ident_t* ident;

	      att = fl->attribute; 
	  
	      if ((ident = identtab_id_lookup(tab,att)) == NULL)
		fprintf(stderr,"failed to serialise field %i (and attribute lookup failed)\n",i);
	      else
		fprintf(stderr,"failed to serialise field %i, attribute %s\n",i,ident->name);
	      
	      return 1;
	    }
	}
 
      rec->n = n;
      rec->fields = f;
    }

  return 0;
} 

static int field_serialise(odb_field_list_t* fl,odb_field_t* f)
{
  f->attribute = fl->attribute;
  f->type      = fl->type;
  f->value     = fl->value;

  return 0;
}

/* 
   odb structure searches 
*/

extern odb_record_t* odb_class_name_lookup(const char* class,identtab_t* tab,odb_t* odb)
{
  ident_t* ident;

  if ((ident = identtab_name_lookup(tab,class)) == NULL)
    {
      fprintf(stderr,"failed lookup of class %s\n",class);
      return NULL;
    }

  printf("class %s -> %i\n",class,ident->id);

  return odb_class_nid_lookup(ident->id,odb);
}

extern odb_record_t* odb_class_nid_lookup(odb_ident_t id,odb_t* odb)
{
  int n,i;
  odb_record_t* rec;

  n   = odb->n;
  rec = odb->records;

  for (i=0 ; i<n ; i++)
    {
      printf("class %i / %i\n",id,rec[i].class);

      if (rec[i].class == id)
	{
	  return rec + i;
	}
    }

  return NULL;
}

extern odb_field_t* odb_attribute_name_lookup(const char* att,identtab_t* tab,odb_record_t* rec)
{
  ident_t* ident;

  if ((ident = identtab_name_lookup(tab,att)) == NULL)
    {
      fprintf(stderr,"failed lookup of attribute %s\n",att);
      return NULL;
    }

  return odb_attribute_nid_lookup(ident->id,rec);
}

extern odb_field_t* odb_attribute_nid_lookup(odb_ident_t id,odb_record_t* rec)
{
  int n,i;
  odb_field_t* f;

  n = rec->n;
  f = rec->fields;

  for (i=0 ; i<n ; i++)
    {
      printf("%i %i\n",f[i].attribute,id);

      if (f[i].attribute == id)
	{
	  printf("found attribute %i at %i\n",id,i);

	  return f + i;
	}
    }

  return NULL;
}
