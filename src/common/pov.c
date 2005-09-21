/*
  pov.h

  structures for povray colour maps, and a couple of
  low-level manipulations

  J.J.Green 2005
  $Id: pov.c,v 1.1 2005/09/20 22:38:58 jjg Exp jjg $
*/

#include <stdlib.h>
#include <string.h>

#include "pov.h"

extern pov_t* pov_new(void)
{
  pov_t* pov;

  if ((pov = malloc(sizeof(pov_t))) == NULL) return NULL;

  pov->n    = 0;
  pov->stop = NULL;
  pov->name[0] = '\0';

  return pov;
}

/* this can only be done once */

extern int pov_stops_alloc(pov_t* pov,int n)
{
  pov_stop_t* stop;

  if ( n<1 ) return 1;

  if ( pov->n > 0) return 1;

  if ((stop = malloc(n*sizeof(pov_stop_t))) == NULL) 
    return 1;
      
  pov->n    = n;
  pov->stop = stop;

  return 0;
}

extern int pov_set_name(pov_t* pov,const char* name)
{
  strncat(pov->name,name,POV_NAME_LEN-1);
  pov->name[POV_NAME_LEN-1] = '\0';

  return 0;
}

extern void pov_destroy(pov_t* pov)
{
  if (pov->n) free(pov->stop);
  free(pov);
}

