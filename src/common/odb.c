/*
  odb.c

  various utility functions for creating/destroying
  odb obhjects

  J.J.Green 2005
  $Id$
*/

#include <stdlib.h>

#include "odb.h"

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

  odb->type = odb_list;
  odb->records.list = list;

  odb->version.major = major;
  odb->version.minor = minor;

  return odb;
}

static void odb_destroy_list(odb_record_list_t*);

extern void odb_destroy(odb_t* odb)
{
  switch (odb->type)
    {
    case odb_list:
      odb_destroy_list(odb->records.list);
      break;
    case odb_serial:
      /* FIXME */
      break;
    default:
      break;
   }

  return;
}

static void odb_destroy_list(odb_record_list_t* rec)
{
  /* FIXME */

  return;
}

/*
  here we serialise the odb abstract syntax tree, converting linked
  lists to arrays, with indexes to speed up searching through it. 
*/

extern int record_serialise(odb_record_list_t*,identtab_t*);

extern int odb_serialise(odb_t* odb,identtab_t* tab)
{
  odb_record_list_t* rec;
  int n;

  if (odb->type != odb_list)
    {
      fprintf(stderr,"attempt to serialise non-list ODB!\n");
      return 0;
    }

  /* traverse the record list */

  for (n=0, rec=odb->records.list ; rec ; rec = rec->next) n++;

  if (!n)
    {
      fprintf(stderr,"ODB has no records\n");
      return 1;
    }

  for (rec = odb->records.list ; rec ; rec = rec->next)
    {
      if (record_serialise(rec,tab) != 0)
	{
	  odb_ident_t class;
	  odb_uint_t  id;
	  ident_t* ident;

	  id    = rec->id;
	  class = rec->class; 
	  
	  if ((ident = identtab_id_lookup(tab,class)) == NULL)
	    fprintf(stderr,"failed to serialise record %i (and class lookup failed)\n",id);
	  else
	    fprintf(stderr,"failed to serialise record %s.%i\n",ident->name,id);

	  return 1;
	}
    }

  odb->type = odb_serial;

  return 0;
}

extern int record_serialise(odb_record_list_t* rec,identtab_t* tab)
{
  return 0;
} 
