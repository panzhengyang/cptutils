/*
  cptclip.h

  J.J.Green 2010
  $Id: cptclip.h,v 1.1 2010/04/04 17:28:11 jjg Exp $
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
