/*
  grd3.h

  paintshop pro gradient structures

  2006 (c) J.J. Green
*/

#ifndef GRD3_H
#define GRD3_H

#include <stdlib.h>

typedef struct
{
  unsigned short z;
  unsigned short r,g,b;
  unsigned short midpoint;
} grd3_rgbseg_t;

typedef struct
{
  unsigned short z;
  unsigned short opacity;
  unsigned short midpoint;
} grd3_opseg_t;

typedef struct
{
  unsigned char* name;
  int ver[2];
  struct 
  {
    int n;
    grd3_rgbseg_t* seg;
  } rgb;
  struct
  {
    int n;
    grd3_opseg_t* seg;
  } op;
} grd3_t;

extern unsigned char grd3magic[];

extern grd3_t* grd3_new(void);
extern void grd3_destroy(grd3_t*);

#endif
 
