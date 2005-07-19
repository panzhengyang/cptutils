/*
  cptio.h

  read/write GMT cpt files 

  (c) J.J.Green 2001,2004
  $Id: cptio.h,v 1.2 2004/03/22 01:09:43 jjg Exp jjg $
*/

#ifndef CPTIO_H
#define CPTIO_H

#include "cpt.h"

extern int cpt_write(const char*,cpt_t*);
extern int cpt_read(const char*,cpt_t*,int);

#endif
