/* 
   scanner for cpt files 

   (c) J.J.Green 2004
   $Id$
*/

%{
#include "cptparse.h"
%}

%option header-file="cptscan.h"
%option outfile="cptscan.c"
%option prefix="cpt"
%option bison-bridge
%option reentrant 
%option 8bit
%option yylineno
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
{NUM}              return NUM;
[Pp]{D}+           return HATCH;
-                  return '-';
F                  return 'F';
B                  return 'B';
N                  return 'N';

%%
