/* 
   scanner for odb files 

   (c) J.J.Green 2005
   $Id: odbscan.l,v 1.1 2005/11/13 23:50:08 jjg Exp jjg $
*/

%{
#include "odb.h"
#include "odbparse.h"

#define YYSTYPE odb_value_t
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
 
%x head

D      [0-9]
H      [0-9a-f]
UINT   {D}+
FLOAT  \-?{D}+\.{D}*
INT    \-{D}+
HEX4   0x{H}{H}{H}{H}
HEX2   0x{H}{H}
IDENT  [a-zA-z]+
STRING \".*\"

%%

"/"                BEGIN(head); return '/';
<head>{UINT}       yylval->u  = (odb_uint_t)atoi(yytext);  return UINT;
<head>"."          return '.';
<head>[ \t]+
<head>"\n"         BEGIN(INITIAL); 
[ \t\n]+
"("                return '(';
")"                return ')';
"."                return '.';
":"                return ':';
"-"                return '-';
{STRING}           return STRING;
{UINT}             yylval->u  = (odb_uint_t)atoi(yytext);  return UINT;
{INT}              yylval->i  = (odb_int_t)atoi(yytext);   return INT;
{FLOAT}            yylval->f  = (odb_float_t)atof(yytext); return FLOAT;
{HEX2}             yylval->h2 = (odb_hex2_t)atoi(yytext);  return HEX2; 
{HEX4}             yylval->h4 = (odb_hex4_t)atoi(yytext);  return HEX4;
{IDENT}            return IDENT;
<<EOF>>            yyterminate();  
.                  printf("didn't expect (%s)\n",yytext);

%%
