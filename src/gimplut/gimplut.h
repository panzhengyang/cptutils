/*
  gimplut.h

  (c) J.J.Green 2008
  $Id: gimplut.h,v 1.1 2008/04/13 20:54:32 jjg Exp $
*/

#ifndef GIMPLUT_H
#define GIMPLUT_H

typedef struct 
{
  size_t verbose, numsamp;
} glopt_t;

extern int gimplut(char*,char*,glopt_t);

#endif





