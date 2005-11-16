/*
  odbparse.y

  a parser for odb files

  (c) J.J.Green 2004
  $Id: odbparse.y,v 1.2 2005/11/15 00:45:15 jjg Exp jjg $
*/

%{
#include "odb.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#include "odbparse.h"
#include "odbscan.h"
#include "odb_bridge.h"

  static void odberror(char const*);

%}

%output="odbparse.c"
%name-prefix="odb"
%defines
%pure-parser
%verbose
%debug

%token UINT INT FLOAT HEX2 HEX4
%token IDENT STRING

%union {
  odb_uint_t uint;
  odb_value_t value;
  odb_ident_t ident;
  odb_field_list_t* field;
  odb_record_list_t* record;
} 

%type <uint>   id
%type <value>  UINT INT FLOAT HEX4 HEX2 STRING IDENT value
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

field : attribute ':' value { $$ = odb_create_field_list($1,$3); }
;

/* basic types */

id : UINT { $$ = $1.u; }
;

value : UINT
| INT 
| FLOAT
| STRING
| HEX4 
| HEX2 
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
