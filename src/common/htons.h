/*
  htons.h

  functions for switching the endianness of integers
  (using library functions if possible)
  
  $Id$
*/

#ifndef HTONS_H
#define HTONS_H

#include "config.h"

#if HAVE_HTONS && HAVE_NTOHS

#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#elif HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#else

#define USE_PORTABLE_ENDIAN

static unsigned short ntohs(unsigned short);
static unsigned short htons(unsigned short);

#endif

#endif
