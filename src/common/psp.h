/*
  psp.h

  read photoshop pro gradients.
  2005 (c) J.J. Green
  $Id$
*/

#ifndef PSP_H
#define PSP_H

#include <stdlib.h>

typedef struct
{
  unsigned int  z;
  unsigned char a,r,g,b;
} psp_seg_t;

typedef struct
{
  char* name;
  int n;
  psp_seg_t* seg;
} psp_grad_t;

extern int read_psp(FILE*,psp_grad_t*);

#endif
