/*
  cptio.h

  read/write GMT cpt files 

  (c) J.J.Green 2001,2004
*/

#ifndef CPTIO_H
#define CPTIO_H

#include "cpt.h"

extern int cpt_write(const char*, cpt_t*);
extern int cpt_read(const char*, cpt_t*);

#endif
