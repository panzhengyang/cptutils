/*
  cptio.h

  read/write GMT cpt files 

  (c) J.J.Green 2001,2004
  $Id: cptio.h,v 1.1 2004/03/04 01:22:13 jjg Exp jjg $
*/

#ifndef CPTIO_H
#define CPTIO_H

#include "cpt.h"

extern int cpt_write(char*,cpt_t*);
extern int cpt_read(char*,cpt_t*,int);

#endif
