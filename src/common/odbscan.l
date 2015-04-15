/* 
   scanner for odb files 

   (c) J.J.Green 2005

   Note that this treats bitblocks badly -- a bitblock may
   be indistinguishable from a UINT, so we scan for a "proper" 
   bitblock like "0a97762f 0928". If we find it we don't
   interpret it, just insert into the string table. Bitblocks
   which look like UINT will be interperted as UINT. That may
   cause a parse error in a higher level structure like AVL.

   These do seem to come in a "Bits" field, so perhaps a better
   way mignt be to make that an ODB keyword, and even interpret
   the bitfield into an appropriate structure.

   I would welcome a patch implementing this functionality

   :-) 
*/

%{

#define YYSTYPE odb_value_t

#include "odb.h"
#include "odbparse.h"
#include "odbbridge.h"
#include "btrace.h"

  static int identifier_insert(char*,identtab_t*);

%}

%option header-file="odbscan.h"
%option outfile="odbscan.c"
%option prefix="odb"
%option bison-bridge
%option yylineno

%option reentrant 
%option 8bit

%option noyywrap
%option noinput
%option nounput
%option debug
 
%x head str

D      [0-9]
H      [0-9a-f]
UINT   {D}+
FLOAT  \-?{D}+\.{D}*
INT    \-{D}+
HEX    0x{H}+
IDENT  [a-zA-z]+
BITBLK {H}{1,8}(" "{H}{1,8})+
STRING \"(.*)\"

%%

"/"                BEGIN(head); return '/';
<head>{UINT}       yylval->u  = (odb_uint_t)atoi(yytext);  return UINT;
<head>"."          return '.';
<head>[ \t]+
<head>[\r\n]+      BEGIN(INITIAL); 
[ \t\r\n]+
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
{BITBLK}           yylval->b     = identifier_insert(yytext,stringtab); return BITBLK;
<<EOF>>            yyterminate();  
.                  btrace("didn't expect (%s)", yytext);

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
	  btrace("failed to create identifier wrapper");
	  return -1;
	}

      if (identtab_insert(tab,ident) != 0)
	{
	  btrace("failed to insert identifier");
	  return -1;
	}

      id = sid++;
    }

  return id;
}
