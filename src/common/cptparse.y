%{
#include "cptparse.h"
#include "cptscan.h"

static void cpterror(char const*);
%}

%output="cptparse.c"
%name-prefix="cpt"
%pure-parser

%%

input : 
      | input line
;

line : '\n'
;

%%


static void cpterror(char const *s)
{
  fprintf (stderr, "%s\n", s);
}

