/*
  pov.h

  structures for povray colour maps, and a couple of
  low-level manipulations

  J.J.Green 2005
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

/*
  here we copy over the name and then normalise the 
  name -- non-alphanumerics are converted to 
  underscore (this needed for the pov-ray parser).
  The count passed to the function has the number
  of charcters changed (caller may wish to warn)
*/

extern int pov_set_name(pov_t* pov,const char* name,int *n)
{
  char *c,*povname = pov->name;
  int m = 0;

  strncat(povname,name,POV_NAME_LEN-1);
  povname[POV_NAME_LEN-1] = '\0';

  for (c = povname, *n = 0 ; *c ; c++)
    if (! (isalnum(*c) || *c == '_'))
      {
	*c = '_';
	m++;
      }

  *n = m;

  return 0;
}

extern void pov_destroy(pov_t* pov)
{
  if (pov->n) free(pov->stop);
  free(pov);
}

