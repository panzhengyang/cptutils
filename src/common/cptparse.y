/*
  cptparse.y

  a forgiving parser for cpt files

  (c) J.J.Green 2004
  $Id$
*/

%{
#include "cptparse-noauto.h"
#include "cptscan.h"

static void cpterror(char const*);
%}

%output="cptparse.c"
%name-prefix="cpt"
%pure-parser
%defines
%verbose

%nonassoc HATCH RGB HSV NUM

%%

input : 
      | model segments extras
;

model : 
      | HSV
      | RGB
;

segments : segment
         | segments segment

segment : z0 val z1 val '\n'

z0 : NUM
z1 : NUM

extras :
       | extras extra
;

extra : 'F' val '\n'
      | 'B' val '\n'
      | 'N' val '\n'
;

val  : '-'
     | HATCH
     | NUM
     | NUM NUM NUM
;

%%

static void cpterror(char const *s)
{
  fprintf (stderr, "%s\n", s);
}

