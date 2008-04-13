/*
  gimplut.h

  (c) J.J.Green 2008
  $Id: gimpcpt.h,v 1.2 2004/02/24 18:36:23 jjg Exp $
*/

#ifndef GIMPLUT_H
#define GIMPLUT_H

typedef struct 
{
  size_t verbose, numsamp;
} glopt_t;

extern int gimplut(char*,char*,glopt_t);

#endif





