/*
  stdcol.h

  J.J. Green 2005
  $Id: stdcol.h,v 1.1 2005/06/26 15:39:08 jjg Exp $
*/

#ifndef STDCOL_H
#define STDCOL_H

struct stdcol_t 
{
  char* name; 
  int r,g,b; 
  double t;
};

extern struct stdcol_t* stdcol(char*);

#endif 
