/*
  avlgrad.h

  read ArcView Legend gradients.
  2005 (c) J.J. Green
  $Id: avlgrad.h,v 1.1 2005/11/22 22:19:14 jjg Exp $
*/

#ifndef AVLGRAD_H
#define AVLGRAD_H

#include <stdlib.h>
#include <stdio.h>

typedef struct
{
  int nodata;
  double min,max;
  unsigned int r,g,b;
} avl_seg_t;

typedef struct
{
  char* name;
  int n;
  avl_seg_t* seg;
} avl_grad_t;

extern int avl_read(FILE*,avl_grad_t*,int,int);
extern int avl_clean(avl_grad_t*);

#endif
