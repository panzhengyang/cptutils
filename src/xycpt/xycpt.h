/*
  xycpt.h

  (c) J.J.Green 2004
  $Id: xycpt.h,v 1.1 2004/04/01 23:37:48 jjg Exp jjg $
*/

#ifndef XYCPT_H
#define XYCPT_H

typedef enum {reg_lower,reg_middle,reg_upper} reg_t;

typedef struct
{
  int verbose;
  int debug;
  int discrete;
  reg_t reg;
  int unital;
  struct
  {
    char *input,*output;
  } file;
} xycpt_opt_t;

extern int xycpt(xycpt_opt_t);

#endif





