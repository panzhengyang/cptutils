/*
  cptcont.h

  J.J.Green 2010
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
