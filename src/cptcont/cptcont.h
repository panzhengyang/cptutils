/*
  cptcont.h

  J.J.Green 2010
  $Id: cptcont.h,v 1.1 2010/04/04 17:28:11 jjg Exp jjg $
*/

#ifndef CPTCONT_H
#define CPTCONT_H

typedef struct
{
  double partial;
  int verbose,midpoint;
} cptcont_opt_t;

extern int cptcont(char*,char*,cptcont_opt_t);

#endif
