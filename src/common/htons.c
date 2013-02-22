/*
  htons.c

  here we define portable ntohs and htons for
  when they are not available in libraries.

  $Id: htons.c,v 1.1 2006/09/01 20:39:51 jjg Exp $
*/

#include "htons.h"

#ifdef USE_PORTABLE_ENDIAN

static unsigned short ntohs(unsigned short x)
{
  unsigned char *c = (unsigned char*)&x;

  return c[0]*256 + c[1]; 
}

static unsigned short htons(unsigned short x)
{
  unsigned char c[2];

  c[0] = x / 256;
  c[1] = x % 256;

  return (unsigned int)c; 
}

#endif
