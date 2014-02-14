/*
  svd.h

  An open linked list to hold an svg gradient, and some 
  operations on them. This does not handle the linear/radial 
  x1/x2 stuff, as we dont use it. Perhaps add later.

  (c) J.J.Green 2001-2005
*/

#ifndef SVG_H
#define SVG_H

#include "colour.h"

typedef struct svg_stop_t
{
  double value;
  double opacity;
  rgb_t  colour;
} svg_stop_t;

typedef struct svg_node_t
{ 
  struct svg_node_t *l,*r;
  svg_stop_t        stop;
} svg_node_t;

#define SVG_NAME_LEN 128

typedef struct svg_t
{
  unsigned char name[SVG_NAME_LEN];
  svg_node_t *nodes;
} svg_t;

extern void   svg_srand(void);
extern svg_t* svg_new();
extern void   svg_init(svg_t*);
extern void   svg_destroy(svg_t*);

extern int svg_prepend(svg_stop_t,svg_t*);
extern int svg_append(svg_stop_t,svg_t*);

extern int svg_each_stop(const svg_t*, int (*)(svg_stop_t*,void*),void*);
extern int svg_interpolate(const svg_t*, double, rgb_t*, double*);
extern int svg_num_stops(const svg_t*);

extern int svg_explicit(svg_t*);
extern int svg_flatten(svg_t*, rgb_t);

#endif




