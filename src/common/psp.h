/*
  psp.h

  read photoshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.h,v 1.1 2005/01/27 00:52:20 jjg Exp jjg $
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
extern int clean_psp(psp_grad_t*);

#endif
