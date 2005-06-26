/*
  svgx.h

  (c) J.J.Green 2004
  $Id: svgx.h,v 1.1 2005/06/24 23:50:38 jjg Exp jjg $
*/

#ifndef SVGX_H
#define SVGX_H

typedef enum {type_cpt, type_ggr} svgx_type_t;

typedef struct
{
  int verbose;
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



