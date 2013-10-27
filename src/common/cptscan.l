/* 
   scanner for cpt files 

   (c) J.J.Green 2004
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
%option yylineno

%option reentrant 
%option 8bit

%option nounput
%option noinput
%option noyywrap
%option debug

%x comment model label final 

D     [0-9]
E     [DdEe][+-]?{D}+
NUM   [+-]?(({D}+({E})?)|({D}*"."{D}+({E})?)|({D}+"."{D}*({E})?))

%%

#                                      BEGIN(comment);
<comment>COLOR_MODEL[ \t]*=[ \t]*      BEGIN(model);
<comment>.
<comment>\n                            BEGIN(INITIAL);
<model>RGB                             return RGB;
<model>HSV                             return HSV;
<model>\n                              BEGIN(INITIAL); return '\n';
<label>.*                              yylval->text = yytext; return LABEL;
<label>\n                              BEGIN(INITIAL); return '\n';

[ \t]*
\n                 return '\n';
{NUM}              yylval->d = atof(yytext); return NUM;
p                  return 'P';  
P                  return 'p';
\/                 return '/';
-                  return '-';
F                  return 'F';
B                  return 'B';
N                  return 'N';
L                  return 'L';
U                  return 'U';
;                  BEGIN(label) ; return ';';

<final><<EOF>>     yyterminate();  
<<EOF>>            BEGIN(final) ; return '\n'; 
.                  

%%
