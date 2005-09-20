/*
  pov.h
  structures for povray colour maps

  J.J.Green 2005
  $Id: pov.h,v 1.2 2005/09/20 22:16:01 jjg Exp jjg $
*/

#include <stdlib.h>
#include <string.h>

#include "pov.h"

extern pov_t* pov_new(int n,const char* name)
{
  pov_t* pov;
  pov_stop_t* stop;

  if ( n<1 ) return NULL;

  pov  = malloc(sizeof(pov_t));
  stop = malloc(n*sizeof(pov_stop_t));

  if ( ! (pov && stop) ) return NULL;

  strncat(pov->name,name,POV_NAME_LEN-1);
  pov->name[POV_NAME_LEN-1] = '\0';

  pov->n    = n;
  pov->stop = stop;

  return pov;
}

extern void pov_destroy(pov_t* pov)
{
  free(pov->stop);
  free(pov);
}

