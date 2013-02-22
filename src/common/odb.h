/*
  odb.h
  structures for obd data
  J.J. Green 2005
  $Id: odb.h,v 1.7 2005/11/22 21:16:55 jjg Exp $
*/

#ifndef ODB_H
#define ODB_H

#include "identtab.h"

typedef int odb_ident_t;

/* value types */

typedef unsigned int     odb_hex_t;
typedef unsigned int     odb_uint_t;
typedef signed int       odb_int_t;
typedef double           odb_float_t;
typedef odb_ident_t      odb_string_t;
typedef odb_ident_t      odb_bitblk_t;

/* union of possible values */

typedef enum 
  { 
    odb_hex,
    odb_uint,
    odb_int,
    odb_float,
    odb_string,
    odb_bitblk,
    odb_ident
  } odb_value_type_t;
	       
typedef union
{
  odb_hex_t    h;
  odb_uint_t   u;
  odb_int_t    i;
  odb_float_t  f;
  odb_string_t s;
  odb_bitblk_t b;
  odb_ident_t  ident;
} odb_value_t;

/* odb in linked list format */

typedef struct odb_field_list_t
{
  odb_ident_t              attribute;
  odb_value_type_t         type;
  odb_value_t              value;
  struct odb_field_list_t *next;
} odb_field_list_t;

extern odb_field_list_t* odb_create_field_list(odb_ident_t,odb_value_t);

typedef struct odb_record_list_t
{
  odb_ident_t               class;
  odb_uint_t                id;
  odb_field_list_t         *fields;
  struct odb_record_list_t *next;
} odb_record_list_t;

extern odb_record_list_t* odb_create_record_list(odb_ident_t,odb_uint_t,odb_field_list_t*);

/* odb in searialised format */

typedef struct
{
  odb_ident_t       attribute;
  odb_value_type_t  type;
  odb_value_t       value;
} odb_field_t;

typedef struct
{
  odb_ident_t    class;
  odb_uint_t     id;
  size_t         n;
  odb_field_t   *fields;
} odb_record_t;

/* odb structure */

typedef struct
{
  /* ODB version, tested against 3.2 */

  struct { odb_uint_t minor,major; } version;

  /* linked list format */

  odb_record_list_t* list;

  /* serialised */

  size_t n;
  odb_record_t* records;

} odb_t;

extern odb_t* odb_create_list(odb_uint_t,odb_uint_t,odb_record_list_t*);
extern int odb_serialise(odb_t*,identtab_t*);

extern odb_record_t* odb_class_name_lookup(const char*,identtab_t*,odb_t*);
extern odb_record_t* odb_class_ident_lookup(odb_ident_t,odb_t*);
extern odb_record_t* odb_class_id_lookup(odb_uint_t,odb_t*);

extern odb_field_t* odb_attribute_name_lookup(const char*,identtab_t*,odb_record_t*);
extern odb_field_t* odb_attribute_ident_lookup(odb_ident_t,odb_record_t*);


extern void odb_destroy(odb_t*);

#endif
