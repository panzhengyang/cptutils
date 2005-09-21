/*
  svgx.h

  (c) J.J.Green 2004
  $Id: svgx.h,v 1.3 2005/08/29 18:26:56 jjg Exp jjg $
*/

#ifndef SVGX_H
#define SVGX_H

typedef enum {type_cpt, type_ggr, type_pov} svgx_type_t;

typedef struct
{
  int verbose;
  int permissive;
  int debug;
  int all;
  int first;
  int list;
  char *name;
  svgx_type_t type; 
  struct
  {
    char *file;
    FILE *stream;;
  } input,output;
} svgx_opt_t;

extern int svgx(svgx_opt_t);

#endif



