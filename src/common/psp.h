/*
  psp.h

  paintshop pro gradient structures

  2006 (c) J.J. Green
  $Id: psp.h,v 1.9 2006/09/01 22:55:16 jjg Exp jjg $
*/

#ifndef PSP_H
#define PSP_H

#include <stdlib.h>

typedef struct
{
  unsigned short z;
  unsigned short r,g,b;
  unsigned short midpoint;
} psp_rgbseg_t;

typedef struct
{
  unsigned short z;
  unsigned short opacity;
  unsigned short midpoint;
} psp_opseg_t;

typedef struct
{
  char* name;
  int ver[2];
  struct 
  {
    int n;
    psp_rgbseg_t* seg;
  } rgb;
  struct
  {
    int n;
    psp_opseg_t* seg;
  } op;
} psp_t;

extern unsigned char pspmagic[];

extern psp_t* psp_new(void);
extern void psp_destroy(psp_t*);

#endif
 
