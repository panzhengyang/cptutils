/*
  psp.h

  read paintshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.h,v 1.5 2006/08/30 23:13:39 jjg Exp jjg $
*/

#ifndef PSP_H
#define PSP_H

#include <stdlib.h>

typedef struct
{
  unsigned int  z;
  unsigned char r,g,b;
  unsigned int  h1; /* unknown meaning */
  unsigned char midpoint;
} psp_rgbseg_t;

typedef struct
{
  unsigned int z;
  unsigned char opacity;
  unsigned char midpoint;
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
 
