/*
  cptio.h

  read/write GMT cpt files 

  (c) J.J.Green 2001,2004
  $Id: cpt.h,v 1.5 2004/02/24 18:37:27 jjg Exp $
*/

#ifndef CPTIO_H
#define CPTIO_H

#include "cpt.h"

extern int cpt_write(char*,cpt_t*);
extern int cpt_read(char*,cpt_t*);

#endif
