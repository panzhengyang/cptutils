/*
  cptparse.y

  a forgiving parser for cpt files

  (c) J.J.Green 2004
  $Id: cptparse.y,v 1.3 2004/03/16 01:26:21 jjg Exp jjg $
*/

%{
#include "cpt.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#include "cptparse.h"
#include "cptscan.h"
#include "bridge.h"

  static void cpterror(char const*);

%}

%output="cptparse.c"
%name-prefix="cpt"
%defines
%pure-parser
%verbose

%union {
  double       d;
  colour_t     colour;
  hatch_t      hatch;
  fill_t       fill;
  cpt_seg_t   *seg;
  cpt_sample_t sample;
} 

%token NUM

%type <d>     NUM z0 z1
%type <fill>  fill 
%type <hatch> hatch
%type <seg>   segment
%nonassoc HATCH RGB HSV

%%

input : 
      | model segments extras
;

model : { bridge->model = rgb; }
| RGB   { bridge->model = rgb; }
| HSV   { bridge->model = hsv; }
;

segments : segment { cpt_append($1,bridge); }
| segments segment { cpt_append($2,bridge); }

segment : z0 fill z1 fill '\n' {
  $$ = cpt_seg_new();

  $$->lsmp.val  = $1;
  $$->lsmp.fill = $2;
  
  $$->rsmp.val  = $3;
  $$->rsmp.fill = $4;
}

z0 : NUM
z1 : NUM

extras :
       | extras extra
;

extra : 'F' fill '\n' { bridge->fg  = $2; }
      | 'B' fill '\n' { bridge->bg  = $2; }
      | 'N' fill '\n' { bridge->nan = $2; }
;

fill  : '-'   { $$.type = empty; }
| hatch       { $$.type = hatch; $$.u.hatch = $1; }
| NUM         { $$.type = grey;  $$.u.grey  = (int)$1; }
| NUM NUM NUM { 
  $$.type = colour;
  switch (bridge->model)
    {
    case hsv:
      $$.u.colour.hsv.hue = $1;
      $$.u.colour.hsv.sat = $2;
      $$.u.colour.hsv.val = $3;
      break;
    case rgb:
      $$.u.colour.rgb.red   = (int)$1;
      $$.u.colour.rgb.green = (int)$2;
      $$.u.colour.rgb.blue  = (int)$3;
    }
}
;

hatch : 'p' NUM '/' NUM {
  $$.sign = 1;
  $$.dpi  = (int)$2;
  $$.n    = (int)$4;
}
| 'P' NUM '/' NUM {
  $$.sign = -1;
  $$.dpi  = (int)$2;
  $$.n    = (int)$4;
}
;

%%

static void cpterror(char const *s)
{
  fprintf (stderr, "%s\n", s);
}

