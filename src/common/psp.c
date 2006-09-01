/*
  psp.c

  paintshop pro gradient structures
  2005 (c) J.J. Green

  $Id: psp.c,v 1.9 2006/09/01 22:32:57 jjg Exp jjg $
*/

#include "psp.h"

unsigned char pspmagic[] = "8BGR";

extern int clean_psp(psp_t* psp)
{
  free(psp->name);
  free(psp->rgb.seg);
  free(psp->op.seg);

  return 0;
}
