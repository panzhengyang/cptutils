/*
  odbparse.y

  a parser for odb files

  (c) J.J.Green 2004
  $Id: odbparse.y,v 1.7 2012/03/09 21:46:11 jjg Exp $
*/

%{
#include "odb.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#include "odbparse.h"
#include "odbscan.h"
#include "odbbridge.h"

  static void odberror(char const*);

%}

%output="odbparse.c"
%name-prefix="odb"
%defines
%pure-parser
%verbose
%debug

%token UINT INT FLOAT HEX
%token IDENT STRING BITBLK

%union {
  odb_uint_t uint;
  odb_value_t value;
  odb_ident_t ident;
  odb_field_list_t* field;
  odb_record_list_t* record;
} 

%type <uint>   id
%type <value>  UINT INT FLOAT HEX STRING IDENT BITBLK
%type <ident>  class attribute
%type <field>  field fields
%type <record> record records

%%

/* final act of parse is to assign to global odb_t structure */

input :  '/' id '.' id records { odb = odb_create_list($2,$4,$5); }
;

/* linked list of records */

records : record
| records record { $2->next = $1; $$ = $2; }
;

record : '(' class '.' id fields ')' { $$ = odb_create_record_list($2,$4,$5); }
| '(' class '.' id ')'               { $$ = odb_create_record_list($2,$4,NULL); }
;

/* linked list of fields */

fields : field
| fields field { $2->next = $1; $$ = $2; }
;

field : attribute ':' UINT   { $$ = odb_create_field_list($1,$3); $$->type = odb_uint; }
| attribute ':' INT    { $$ = odb_create_field_list($1,$3); $$->type = odb_int; }
| attribute ':' HEX    { $$ = odb_create_field_list($1,$3); $$->type = odb_hex; }
| attribute ':' FLOAT  { $$ = odb_create_field_list($1,$3); $$->type = odb_float; }
| attribute ':' STRING { $$ = odb_create_field_list($1,$3); $$->type = odb_string; }
| attribute ':' BITBLK { $$ = odb_create_field_list($1,$3); $$->type = odb_bitblk; }
;

/* basic types */

id : UINT { $$ = $1.u; }
;

class : IDENT { $$ = $1.ident; }
;

attribute : IDENT { $$ = $1.ident; }
;  

%%

static void odberror(char const *s)
{
  fprintf(stderr, "%s\n",s);
}
