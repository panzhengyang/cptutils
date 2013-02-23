/*
  cptclip.h

  J.J.Green 2010
*/

#ifndef CPTCLIP_H
#define CPTCLIP_H

#include <stdbool.h>

typedef struct
{
  bool segments,verbose;
  union
  {
    struct {
      double min,max;
    } z;
    struct {
      size_t min,max;
    } segs;
  } u;
} cptclip_opt_t;

extern int cptclip(char*,char*,cptclip_opt_t);

#endif
