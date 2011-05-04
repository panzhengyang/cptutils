/*
  css3.h
  structures for css3 gradients

  J.J.Green 2010
  $Id: css3.h,v 1.2 2010/11/01 19:21:28 jjg Exp $
*/

#ifndef CSS3_H
#define CSS3_H

#include "colour.h"

typedef struct 
{
  double z;
  rgb_t rgb;   
} css3_stop_t;

typedef struct 
{
  double angle;
  int n;
  css3_stop_t *stop;
} css3_t;

extern css3_t* css3_new(void);
extern void css3_destroy(css3_t*);

extern int css3_stops_alloc(css3_t*,int);

#endif
