/*
  psp.h

  read photoshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.h,v 1.2 2005/01/27 21:32:02 jjg Exp jjg $
*/

#ifndef PSP_H
#define PSP_H

#include <stdlib.h>

typedef struct
{
  unsigned int  z;
  unsigned char a,r,g,b;
  /* smoothess parameter? */
  unsigned int h1;
  /* might be the midpoint */
  unsigned char h2;
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
