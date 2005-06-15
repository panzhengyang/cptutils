/*
  svglist.h

  opaque lists of svg gradients

  J.J.Green 2005
*/

#ifndef SVGLIST_H
#define SVGLIST_H

#include "svg.h"

typedef struct svg_list_t svg_list_t;

extern svg_list_t* svg_list_new(void);
extern void svg_list_destroy(svg_list_t*);

/*
  returns a pointer to an unused svg_t on the list,
  its is expected that the caller will fill in the
  details on the svg_t (add stops, etc)

  the list will expand dynamically when large
  numbers of calls on this function
*/

extern svg_t* svg_list_svg(svg_list_t*);

/*
  return the number of svg_t stuctures in the list
*/

extern int svg_list_size(svg_list_t*);

/*
  applies the function to each svg_t in the list, the
  function should return 0 on success. If it returns
  non-zero then the iteration will terminate an the
  value returned
*/

extern int svg_list_iterate(svg_list_t*,int (*)(svg_t*,void*),void*);

#endif
