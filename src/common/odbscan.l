/* 
   scanner for odb files 

   (c) J.J.Green 2005
   $Id: cptscan.l,v 1.5 2004/03/22 01:09:28 jjg Exp $
*/

%{
#include "odb.h"
#include "odbparse.h"
%}

%option header-file="odbscan.h"
%option outfile="odbscan.c"
%option prefix="odb"
%option bison-bridge
%option yylineno

%option reentrant 
%option 8bit

%option noyywrap
%option debug

D      [0-9]
H      [0-9a-f]
UINT   {D}+
HEX4   0x{H}{H}{H}{H}
HEX2   0x{H}{H}
IDENT  [a-zA-z]+
STRING \".*\"

%%

[ \t]+
"/"                return '/';
"("                return '(';
")"                return ')';
"."                return '.';
":"                return ':';
{STRING}           return STRING;
{UINT}             yylval->d = atoi(yytext); return UINT;
{HEX2}             yylval->d = atoi(yytext); return HEX2;
{HEX4}             yylval->d = atoi(yytext); return HEX4;
{IDENT}            return IDENT;
<<EOF>>            yyterminate();  
.                  printf("didn't expect (%s)\n",yytext);

%%
