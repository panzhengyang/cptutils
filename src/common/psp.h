/*
  psp.h

  read paintshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.h,v 1.6 2006/08/30 23:41:24 jjg Exp jjg $
*/

#ifndef PSP_H
#define PSP_H

#include <stdlib.h>

typedef struct
{
  unsigned short z;
  unsigned short r,g,b;
  unsigned short h1; /* unknown meaning */
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
} psp_grad_t;

extern int read_psp(FILE*,psp_grad_t*);
extern int clean_psp(psp_grad_t*);

#endif
 
