/*
  psp.c

  paintshop pro gradient structures
  2005 (c) J.J. Green
*/

#include "psp.h"

unsigned char pspmagic[] = "8BGR";

extern psp_t* psp_new(void)
{
  psp_t* psp;

  if ((psp = malloc(sizeof(psp_t))) == NULL)
    return NULL;

  psp->name    = NULL;

  /* 
     all psp files seem to have this version (and psp x will
     not load a file which does not have it), but the caller
     can modify it
  */

  psp->ver[0]  = 3;
  psp->ver[1]  = 1;

  psp->rgb.n   = 0;
  psp->rgb.seg = NULL;

  psp->op.n    = 0;
  psp->op.seg  = NULL;

  return psp;
}

extern void psp_destroy(psp_t* psp)
{
  free(psp->name);
  free(psp->rgb.seg);
  free(psp->op.seg);

  free(psp);
}
