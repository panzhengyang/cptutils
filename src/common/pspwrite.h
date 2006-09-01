/*
  pspwrite.h

  writes paintshop pro gradients.
  2006 (c) J.J. Green
  $Id: pspwrite.h,v 1.2 2006/09/01 20:46:51 jjg Exp jjg $
*/

#ifndef PSPWRITE_H
#define PSPWRITE_H

#include <stdio.h>

#include "psp.h"

extern int psp_write(FILE*,psp_grad_t*);

#endif
 
