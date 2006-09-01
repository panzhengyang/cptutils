/*
  pspwrite.h

  writes paintshop pro gradients.
  2006 (c) J.J. Green
  $Id: pspwrite.h,v 1.1 2006/09/01 20:15:09 jjg Exp jjg $
*/

#ifndef PSPWRITE_H
#define PSPWRITE_H

#include <stdio.h>

#include "psp.h"

extern int write_psp(FILE*,psp_grad_t*);

#endif
 
