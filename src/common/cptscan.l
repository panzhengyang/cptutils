/* 
   scanner for cpt files 

   (c) J.J.Green 2004
   $Id: cptscan.l,v 1.3 2004/03/16 01:26:57 jjg Exp jjg $
   %option bison-bridge
*/

%{
#include "cpt.h"
#include "cptparse.h"
%}

%option header-file="cptscan.h"
%option outfile="cptscan.c"
%option prefix="cpt"
%option bison-bridge

%option reentrant 
%option 8bit

%option noyywrap
%option debug

%x comment model

D     [0-9]
E     [DdEe][+-]?{D}+
NUM   ({D}+({E})?)|({D}*"."{D}+({E})?)|({D}+"."{D}*({E})?)

%%

#                                      BEGIN(comment);
<comment>COLOR_MODEL[ \t]*=[ \t]*      BEGIN(model);
<comment>.
<comment>\n                            BEGIN(INITIAL);
<model>RGB                             return RGB;
<model>HSV                             return HSV;
<model>\n                              BEGIN(INITIAL);

[ \t]*
\n                 return '\n';
{NUM}              yylval->d =  atof(yytext); return NUM;
p                  return 'P';  
P                  return 'p';
\/                 return '/';
-                  return '-';
F                  return 'F';
B                  return 'B';
N                  return 'N';

%%
