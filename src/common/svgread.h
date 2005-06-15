/*
  svgread.h

  reads svg gradient files - a list of svg_t structs is 
  returned (since a single svg file may contain several 
  svg gradients)

  J.J. Green 2005
*/

#ifndef SVGREAD_H
#define SVGREAD_H

#include <stdlib.h>

#include "svglist.h"

extern int svg_read(FILE*,svg_list_t*);

#endif
