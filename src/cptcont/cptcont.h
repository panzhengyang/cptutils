/*
  cptcont.h

  J.J.Green 2010
  $Id: cptcont.h,v 1.2 2010/04/18 16:20:05 jjg Exp $
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
