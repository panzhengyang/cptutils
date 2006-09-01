/*
  psp.c

  paintshop pro gradient structures
  2005 (c) J.J. Green

  $Id: psp.c,v 1.10 2006/09/01 22:55:51 jjg Exp jjg $
*/

#include "psp.h"

unsigned char pspmagic[] = "8BGR";

extern psp_t* psp_new(void)
{
  psp_t* psp;

  if ((psp = malloc(sizeof(psp_t))) == NULL)
    return NULL;

  psp->name    = NULL;
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

  return 0;
}
