/*
  odbparse.y

  a parser for odb files

  (c) J.J.Green 2004
  $Id: odbparse.y,v 1.1 2005/11/13 23:50:02 jjg Exp jjg $
*/

%{
#include "odb.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#define YYSTYPE odb_value_t

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
  odb_value_t value;
  odb_field_list_t* field;
  odb_recordlist_t* record;
} 

%type <value> UINT INT FLOAT HEX4 HEX2 STRING value
%type <field> field fields

%%

input :  header records
;

records : record
| records record
;

header : '/' version
;

version : UINT '.' UINT 
;

record : '(' class '.' id fields ')'
;

fields : /* empty */ 
| field
| fields field
;

id : UINT
;

field : attribute ':' value
;

value : UINT
| INT 
| FLOAT
| STRING
| HEX4 
| HEX2 
;

class : IDENT
;

attribute : IDENT
;  

%%

static void odberror(char const *s)
{
  fprintf (stderr, "%s\n",s);
}
