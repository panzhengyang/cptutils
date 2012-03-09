/*
  cptparse.y

  a forgiving parser for cpt files

  (c) J.J.Green 2004
  $Id: cptparse.y,v 1.6 2005/03/30 23:10:08 jjg Exp jjg $
*/

%{
#include "cpt.h"

#define YYLEX_PARAM scanner 
#define YYPARSE_PARAM scanner 

#include "cptparse.h"
#include "cptscan.h"
#include "cptbridge.h"

  static void cpterror(char const*);
  static cpt_sample_t sample3(double,double,double,double);
  static cpt_sample_t sample1(double,double);
  static cpt_sample_t sampleh(double,hatch_t);
  static cpt_sample_t samplee(double);
%}

%output="cptparse.c"
%name-prefix="cpt"
%defines
%pure-parser
%verbose
%debug

%union {
  double       d;
  annote_t     annote;
  colour_t     colour;
  hatch_t      hatch;
  fill_t       fill;
  cpt_seg_t   *seg;
  cpt_sample_t sample;
} 

%token NUM

%type <d>      NUM 
%type <fill>   fill 
%type <hatch>  hatch
%type <seg>    spline segment
%type <annote> annote
%nonassoc HATCH RGB HSV 

%%

input : lines 
;

lines : line
| lines line
;

line : content '\n'
| '\n'
;

content : segment { cpt_append($1,bridge); }
| model
| extra 
;

model :  RGB { bridge->model = rgb; }
| HSV { bridge->model = hsv; }
;

segment : spline {
  $$ = $1;
  $$->annote = none;
}
| spline annote {
  $$ = $1;
  $$->annote = $2;
}
;

annote : 'L' { $$ = lower; }
|        'U' { $$ = upper; }
|        'B' { $$ = both ; }
;

spline : NUM NUM NUM NUM NUM NUM NUM NUM {
  $$ = cpt_seg_new();
  $$->lsmp = sample3($1,$2,$3,$4);
  $$->rsmp = sample3($5,$6,$7,$8);
}
| NUM NUM NUM NUM {
  $$ = cpt_seg_new();
  $$->lsmp = sample1($1,$2);
  $$->rsmp = sample1($3,$4);
}
| NUM hatch NUM hatch {
  $$ = cpt_seg_new();
  $$->lsmp = sampleh($1,$2);
  $$->rsmp = sampleh($3,$4);
} 
| NUM hatch NUM '-' {
  $$ = cpt_seg_new();
  $$->lsmp = sampleh($1,$2);
  $$->rsmp = sampleh($3,$2);
} 
| NUM '-' NUM '-' {
  $$ = cpt_seg_new();
  $$->lsmp = samplee($1);
  $$->rsmp = samplee($3);
}
; 

extra : 'F' fill { bridge->fg  = $2; }
      | 'B' fill { bridge->bg  = $2; }
      | 'N' fill { bridge->nan = $2; }
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
  fprintf (stderr, "%s\n",s);
}

static cpt_sample_t sample3(double z,double c1,double c2,double c3)
{
  cpt_sample_t s;
  fill_t fill;

  fill.type = colour;

  switch (bridge->model)
    {
    case hsv:
      fill.u.colour.hsv.hue = c1;
      fill.u.colour.hsv.sat = c2;
      fill.u.colour.hsv.val = c3;
      break;
    case rgb:
      fill.u.colour.rgb.red   = (int)c1;
      fill.u.colour.rgb.green = (int)c2;
      fill.u.colour.rgb.blue  = (int)c3;
    }

  s.val  = z;
  s.fill = fill;

  return s;
}

static cpt_sample_t sample1(double z,double g)
{
  cpt_sample_t s;
  fill_t fill;

  fill.type   = grey;
  fill.u.grey = (int)g;

  s.val  = z;
  s.fill = fill;

  return s;
}

static cpt_sample_t sampleh(double z,hatch_t h)
{
  cpt_sample_t s;
  fill_t fill;

  fill.type    = hatch;
  fill.u.hatch = h;

  s.val  = z;
  s.fill = fill;

  return s;
}

static cpt_sample_t samplee(double z)
{
  cpt_sample_t s;

  s.val = z;
  s.fill.type = empty;

  return s;
}
