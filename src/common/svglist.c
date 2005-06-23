/*
  svglist.c

  lists of svg gradients

  J.J.Green 2005
*/

#include <stdlib.h>

#include "svglist.h"

struct svg_list_t
{
  int    n,alloc;
  svg_t* svg;
};

#define LIST_INI 20
#define LIST_INC 20

extern svg_list_t* svg_list_new(void)
{
  svg_list_t *list;
  svg_t *svg;

  list = malloc(sizeof(svg_list_t));
  svg  = malloc(sizeof(svg_t)*LIST_INI);

  if ( ! (list && svg) ) return NULL;

  list->svg   = svg;
  list->alloc = LIST_INI;
  list->n     = 0;

  return list;
}

extern void svg_list_destroy(svg_list_t* list)
{
  free(list->svg);
  free(list);

  return;
}

extern int svg_list_iterate(svg_list_t* list,int (*f)(svg_t*,void*),void* opt)
{
  int i,n;
  svg_t* svg;

  n = list->n;

  for (i=0,svg = list->svg ; i<n ; i++,svg++)
    {
      int err = f(svg,opt);

      if (err) return err;
    }

  return 0;
}

extern svg_t* svg_list_select(svg_list_t* list,int (*f)(svg_t*,void*),void* opt)
{
  int i,n;
  svg_t* svg;
  
  n = list->n;
  
  for (i=0,svg=list->svg ; i<n ; i++,svg++)
    {
      if (f(svg,opt) != 0)  return svg;
    }

  return NULL;
}

extern svg_t* svg_list_svg(svg_list_t* list)
{
  int n,a;
  svg_t *svg;

  n = list->n;
  a = list->alloc;

  /* return next svg_t free */

  if (n < a)
    {
      list->n++;

      return list->svg + n; 
    }

  /* not enough room, so reallocate */

  a += LIST_INC;

  svg = realloc(list->svg,a);

  if (svg == NULL) return NULL;

  list->alloc = a;

  return svg_list_svg(list);
}

extern int svg_list_size(svg_list_t* list)
{
  return list->n;
}
