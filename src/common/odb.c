/*
  odb.c

  various utility functions for creating/destroying
  odb obhjects

  J.J.Green 2005
*/

#include <stdlib.h>

#include "odb.h"
#include "btrace.h"

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
      btrace("attempt to serialise ODB without list!");
      return 0;
    }

  for (n=0, recls=odb->list ; recls ; recls = recls->next) n++;

  if (!n)
    {
      btrace("ODB has no records");
      return 1;
    }

  if ((rec = malloc(n*sizeof(odb_record_t))) == NULL)
    {
      btrace("out of memory");
      return 1;
    }

  for (recls = odb->list, i=0 ; recls ; recls = recls->next, i++)
    {
      /* reverse the linked list order */

      if (record_serialise(recls,tab,rec+(n-1-i)) != 0)
	{
	  odb_ident_t class;
	  odb_uint_t  id;
	  ident_t* ident;

	  id    = recls->id;
	  class = recls->class; 
	  
	  if ((ident = identtab_id_lookup(tab,class)) == NULL)
	    btrace("failed to serialise record %i (and class lookup failed)",id);
	  else
	    btrace("failed to serialise record %s.%i",ident->name,id);

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

  rec->id    = recls->id;
  rec->class = recls->class;

  rec->n = 0;
  rec->fields = NULL;

  for (n=0,fl=recls->fields ; fl ; fl=fl->next) n++; 

  if (n)
    {
      int i;

      if ((f = malloc(n*sizeof(odb_field_t))) == NULL)
	{
	  btrace("out of memory");
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
		btrace("failed to serialise field %i (and attribute lookup failed)",i);
	      else
		btrace("failed to serialise field %i, attribute %s",i,ident->name);
	      
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
   odb structure searches - we just do a linear search, there are only going to
   be a few hundred of these at most. this is less painful than you would think
   since there are no strcmp() calls, just integer compares (due to use of the
   identifier table)
*/

extern odb_record_t* odb_class_name_lookup(const char* class,identtab_t* tab,odb_t* odb)
{
  ident_t* ident;

  if ((ident = identtab_name_lookup(tab,class)) == NULL)
    {
      btrace("failed lookup of class %s",class);
      return NULL;
    }

  return odb_class_ident_lookup(ident->id,odb);
}

extern odb_record_t* odb_class_ident_lookup(odb_ident_t id,odb_t* odb)
{
  int n,i;
  odb_record_t* rec;

  n   = odb->n;
  rec = odb->records;

  for (i=0 ; i<n ; i++)
    {
      if (rec[i].class == id)
	{
	  return rec + i;
	}
    }

  return NULL;
}

extern odb_record_t* odb_class_id_lookup(odb_uint_t id,odb_t* odb)
{
  int n,i;
  odb_record_t* rec;

  n   = odb->n;
  rec = odb->records;

  for (i=0 ; i<n ; i++)
    {
      if (rec[i].id == id)
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
      btrace("failed lookup of attribute %s",att);
      return NULL;
    }

  return odb_attribute_ident_lookup(ident->id,rec);
}

extern odb_field_t* odb_attribute_ident_lookup(odb_ident_t id,odb_record_t* rec)
{
  int n,i;
  odb_field_t* f;

  n = rec->n;
  f = rec->fields;

  for (i=0 ; i<n ; i++)
    {
      if (f[i].attribute == id)
	{
	  return f + i;
	}
    }

  return NULL;
}
