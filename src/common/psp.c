/*
  psp.c

  paintshop pro gradient structures
  2005 (c) J.J. Green

  $Id: psp.c,v 1.8 2006/08/31 23:23:49 jjg Exp jjg $
*/

#include "psp.h"

unsigned char pspmagic[] = "8BGR";

extern int clean_psp(psp_grad_t* grad)
{
  free(grad->name);
  free(grad->rgb.seg);
  free(grad->op.seg);

  return 0;
}
