/*
  cptparse.y

  a forgiving parser for cpt files

  (c) J.J.Green 2004
  $Id: cptparse.y,v 1.2 2004/03/05 01:27:44 jjg Exp jjg $
*/

%{

#include "cpt.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#include "cptparse.h"
#include "cptscan.h"

  static model_t model;
  static filltype_t filltype;
  static void cpterror(char const*);

%}

%output="cptparse.c"
%name-prefix="cpt"
%defines
%pure-parser
%verbose

%union {
  double       d;
  double       d3[3];
  int          i;
  colour_t     colour;
  fill_t       fill;
  cpt_sample_t sample;
} 

%token NUM

%type <d>    NUM z0 z1
%type <fill> fill 
%type <i>    HATCH
%nonassoc HATCH RGB HSV

%%

input : 
      | model segments extras
;

model : 
| HSV { model = hsv; }
| RGB { model = rgb; }
;

segments : segment
         | segments segment

segment : z0 fill z1 fill '\n'

z0 : NUM
z1 : NUM

extras :
       | extras extra
;

extra : 'F' fill '\n'
      | 'B' fill '\n'
      | 'N' fill '\n'
;

fill  : '-'   { filltype = empty; }
| HATCH       { filltype = hatch; $$.hatch = (int)$1; }
| NUM         { filltype = grey;  $$.grey  = (int)$1; }
| NUM NUM NUM { 
  filltype = colour;
  switch (model)
    {
    case hsv:
      $$.colour.hsv.hue = $1;
      $$.colour.hsv.sat = $2;
      $$.colour.hsv.val = $3;
      break;
    case rgb:
      $$.colour.rgb.red   = (int)$1;
      $$.colour.rgb.green = (int)$2;
      $$.colour.rgb.blue  = (int)$3;
    }
}
;

%%

static void cpterror(char const *s)
{
  fprintf (stderr, "%s\n", s);
}

