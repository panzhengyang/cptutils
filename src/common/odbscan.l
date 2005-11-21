/* 
   scanner for odb files 

   (c) J.J.Green 2005
   $Id: odbscan.l,v 1.4 2005/11/19 00:39:05 jjg Exp jjg $
*/

%{
#include "odb.h"
#include "odbparse.h"
#include "odb_bridge.h"

#define YYSTYPE odb_value_t

  static int identifier_insert(char*,identtab_t*);

%}

%option header-file="odbscan.h"
%option outfile="odbscan.c"
%option prefix="odb"
%option bison-bridge
%option yylineno

%option reentrant 
%option 7bit

%option noyywrap
%option debug
 
%x head str

D      [0-9]
H      [0-9a-f]
UINT   {D}+
FLOAT  \-?{D}+\.{D}*
INT    \-{D}+
HEX    0x{H}+
IDENT  [a-zA-z]+
STRING \"(.*)\"

%%

"/"                BEGIN(head); return '/';
<head>{UINT}       yylval->u  = (odb_uint_t)atoi(yytext);  return UINT;
<head>"."          return '.';
<head>[ \t]+
<head>"\n"         BEGIN(INITIAL); 
[ \t\n]+
\"                 BEGIN(str);
<str>[^\"]*        yylval->s = identifier_insert(yytext,stringtab); return STRING;
<str>\"            BEGIN(INITIAL);
"("                return '(';
")"                return ')';
"."                return '.';
":"                return ':';
{UINT}             yylval->u     = (odb_uint_t)atoi(yytext);  return UINT;
{INT}              yylval->i     = (odb_int_t)atoi(yytext);   return INT;
{HEX}              yylval->h     = (odb_hex_t)strtol(yytext,NULL,16);  return HEX;
{FLOAT}            yylval->f     = (odb_float_t)atof(yytext); return FLOAT;
{IDENT}            yylval->ident = identifier_insert(yytext,identtab); return IDENT;
<<EOF>>            yyterminate();  
.                  fprintf(stderr,"didn't expect (%s)\n",yytext);

%%

/*
  this returns the (unique) id of the name inserted, 
  or -1 on error 
*/

static int identifier_insert(char* name,identtab_t* tab)
{
  int id;
  ident_t* ident;

  ident = identtab_name_lookup(tab,name); 

  if (ident)
    {
      id = ident->id;
    }
  else
    {
      static int sid = 1;
      
      if ((ident = ident_new(name,sid)) == NULL)
	{
	  fprintf(stderr,"failed to create identifier wrapper\n");
	  return -1;
	}

      if (identtab_insert(tab,ident) != 0)
	{
	  fprintf(stderr,"failed to insert identifier\n");
	  return -1;
	}

      id = sid++;
    }

  return id;
}
