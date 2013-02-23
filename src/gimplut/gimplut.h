/*
  gimplut.h

  (c) J.J.Green 2008
*/

#ifndef GIMPLUT_H
#define GIMPLUT_H

typedef struct 
{
  size_t verbose, numsamp;
} glopt_t;

extern int gimplut(char*,char*,glopt_t);

#endif





