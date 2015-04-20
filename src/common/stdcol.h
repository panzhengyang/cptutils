/*
  stdcol.h

  J.J. Green 2005
*/

#ifndef STDCOL_H
#define STDCOL_H

struct stdcol_t 
{
  char* name; 
  int r, g, b; 
  double t;
};

extern const struct stdcol_t* stdcol(char*);

#endif 
