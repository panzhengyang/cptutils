/*
  gimpcpt.h

  (c) J.J.Green 2001
  $Id: gimpcpt.h,v 1.5 2001/05/24 01:04:17 jjg Exp $
*/

#ifndef GIMPCPT_H
#define GIMPCPT_H

#include "colour.h"

typedef struct cptopt_t
{
    unsigned int verbose    : 1;
    unsigned int reverse    : 1;

    rgb_t  fg,bg,nan,trans;

    int samples;

    double min;
    double max;

} cptopt_t;

extern int gimpcpt(char*,char*,cptopt_t);

#endif





