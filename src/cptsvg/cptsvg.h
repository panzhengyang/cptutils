/*
  cptsvg.h

  J.J.Green 2004
  $Id: cptsvg.h,v 1.1 2004/02/11 00:58:29 jjg Exp $
*/

#ifndef CPTSVG_H
#define CPTSVG_H

typedef struct cptsvg_opt_t
{
  int verbose;
} cptsvg_opt_t;

extern int cptsvg(char*,char*,cptsvg_opt_t);

#endif
