/*
  cptcont.h

  J.J.Green 2010
  $Id: cptcont.h,v 1.1 2007/11/16 17:05:49 jjg Exp $
*/

#ifndef CPTCONT_H
#define CPTCONT_H

typedef struct
{
  double partial;
  int verbose;
} cptcont_opt_t;

extern int cptcont(char*,char*,cptcont_opt_t);

#endif
