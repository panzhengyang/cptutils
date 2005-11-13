/*
  odbparse.y

  a parser for odb files

  (c) J.J.Green 2004
  $Id: cptparse.y,v 1.6 2005/03/30 23:10:08 jjg Exp $
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

%union {
  double d;
  int    i;
} 

%token UINT
%token HEX2
%token HEX4
%token IDENT
%token STRING

%type <d> UINT 

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

value : inttype 
| floattype 
| STRING
| HEX4 
| HEX2 
;

inttype : UINT
| '-' UINT
;

floattype : inttype '.' UINT
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
