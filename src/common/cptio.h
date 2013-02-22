/*
  cptio.h

  read/write GMT cpt files 

  (c) J.J.Green 2001,2004
  $Id: cptio.h,v 1.3 2005/07/19 22:21:44 jjg Exp $
*/

#ifndef CPTIO_H
#define CPTIO_H

#include "cpt.h"

extern int cpt_write(const char*,cpt_t*);
extern int cpt_read(const char*,cpt_t*,int);

#endif
