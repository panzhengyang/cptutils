/*
  gimpsvg.h

  (c) J.J.Green 2011
  $Id: gimpsvg.h,v 1.2 2004/02/24 18:36:23 jjg Exp jjg $
*/

#ifndef GIMPSVG_H
#define GIMPSVG_H

#include "colour.h"

typedef struct 
{
  unsigned int verbose    : 1;
  unsigned int reverse    : 1;
  
  rgb_t  fg,bg,nan,trans;
  
  int samples;
  
  double min;
  double max;
  
  double tol;

} gimpsvg_opt_t;

extern int gimpsvg(const char*, const char*, gimpsvg_opt_t);

#endif





