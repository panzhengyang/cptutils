/*
  odb.h
  structures for obd data
  J.J. Green 2005
  $Id: odb.h,v 1.1 2005/11/13 23:50:33 jjg Exp jjg $
*/

#ifndef ODB_H
#define ODB_H

typedef unsigned int odb_identifier_t;

/* value types */

typedef unsigned short   odb_hex2_t;
typedef unsigned int     odb_hex4_t;
typedef unsigned int     odb_uint_t;
typedef signed int       odb_int_t;
typedef double           odb_float_t;
typedef unsigned int     odb_xref_t;
typedef odb_identifier_t odb_string_t;

/* union of possible values */

typedef enum 
  { 
    odb_hex2,
    odb_hex4,
    odb_uint,
    odb_int,
    odb_float,
    odb_xref,
    odb_string
  } odb_value_type_t;
	       
typedef union
{
  odb_hex2_t   h2;
  odb_hex4_t   h4;
  odb_uint_t   u;
  odb_int_t    i;
  odb_float_t  f;
  odb_xref_t   xref;
  odb_string_t s;
} odb_value_t;

/* odb in linked list format */

typedef struct odb_field_list_t
{
  odb_identifier_t         attribute;
  odb_value_type_t         type;
  odb_value_t              value;
  struct odb_field_list_t *next;
} odb_field_list_t;

typedef struct odb_record_list_t
{
  odb_identifier_t          class;
  odb_uint_t                id;
  odb_field_list_t         *fields;
  struct odb_record_list_t *next;
} odb_record_list_t;

typedef struct
{
  struct 
  {
    odb_uint_t minor,major;  
  } version;
  odb_record_list_t* records;
} odb_list_t;

/* odb serialised, for later */

typedef struct
{
  struct 
  {
    odb_uint_t minor,major;  
  } version;
} odb_serial_t;

/* odb structure */

typedef enum {odb_list,odb_serial} odb_struct_t;

typedef struct
{
  odb_struct_t type;
  union
  {
    odb_serial_t serial;
    odb_list_t   list;
  } u;
} odb_t;

#endif
