/*
  xycpt.h

  (c) J.J.Green 2004
  $Id: gimpcpt.h,v 1.2 2004/02/24 18:36:23 jjg Exp $
*/

#ifndef XYCPT_H
#define XYCPT_H

typedef struct
{
  int verbose;
  int debug;
  int discrete;
  int unital;
  struct
  {
    char *input,*output;
  } file;
} xycpt_opt_t;

extern int xycpt(xycpt_opt_t);

#endif





