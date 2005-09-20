/*
  svd.c

  An open linked list to hold an svg gradient, and some 
  operations on them. This does not handle the linear/radial 
  x1/x2 stuff, as we dont use it. Perhaps add later.

  (c) J.J.Green 2001-2005
  $Id: svg.c,v 1.2 2005/06/02 22:39:50 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "svg.h"

/*
  this operates on each stop : if f returns 
  0 - continue
  1 - short circuit success  
  x - short circuit failure, return x

  not tested
*/

extern int svg_each_stop(svg_t* svg,int (*f)(svg_stop_t,void*),void* opt)
{
  svg_node_t *node;

  for (node = svg->nodes ; node ; node = node->r)
    {
      int err = f(node->stop,opt);

      switch (err)
	{
	case 0:  break;
	case 1:  return 0;
	default: return err;
	}
    } 

  return 0;
}

extern svg_t* svg_new(void)
{
    svg_t* svg;

    if ((svg = malloc(sizeof(svg_t))) == NULL)
      return NULL;

    svg->nodes = NULL;

    return svg;
}

static svg_node_t* svg_node_new(svg_stop_t stop)
{
  svg_node_t *n;

  if ((n = malloc(sizeof(svg_node_t))) == NULL) return NULL;
  
  n->stop = stop;

  return n;
}

static void svg_node_destroy(svg_node_t* node)
{
    free(node);
}

extern int svg_prepend(svg_stop_t stop,svg_t* svg)
{
    svg_node_t *node,*root;

    root = svg->nodes;

    if ((node = svg_node_new(stop)) == NULL) return 1;

    node->l = NULL;
    node->r = root;

    if (root) root->l = node;

    svg->nodes = node;

    return 0;
}

extern int svg_append(svg_stop_t stop,svg_t* svg)
{
    svg_node_t *node,*n;

    n = svg->nodes;

    if (!n) 
      return svg_prepend(stop,svg);
 
    while (n->r) 
      n = n->r;
 
    if ((node = svg_node_new(stop)) == NULL) 
      return 1;

    n->r    = node;
    node->l = n;
    node->r = NULL;
 
    return 0;
}

extern void svg_destroy(svg_t* svg)
{
    svg_node_t *node, *next;

    if (!svg) return;
    
    if ((node = svg->nodes) != NULL)
      {
	while (node) 
	  {
	    next = node->r;
	    svg_node_destroy(node);
	    node = next;
	  }
      }
 
    free(svg);

    return;
}

static int inc(svg_stop_t stop,int* n)
{
  (*n)++;

  return 0;
}

extern int svg_num_stops(svg_t* svg)
{
  int n = 0;

  if (svg_each_stop(svg,(int (*)(svg_stop_t,void*))inc,&n) != 0) return -1;

  return n;
}

