/* Scanner for "C" assignment statements... sort of. */
%{
#include "cptparse.h"
%}
%option header-file="cptscan.h"
%option outfile="cptscan.c"
%option prefix="cpt"
%option bison-bridge
%option reentrant 
%option case-insensitive
%option 8bit
%option yylineno
%option yywrap
%%
# 
%%
