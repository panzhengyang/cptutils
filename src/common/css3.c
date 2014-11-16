/*
  css3.h

  structures for css3 gradients, and a couple of
  low-level manipulations

  J.J.Green 2011
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "css3.h"
#include "btrace.h"

extern css3_t* css3_new(void)
{
  css3_t* css3;

  if ((css3 = malloc(sizeof(css3_t))) == NULL)
    {
      btrace("failed memory allocation");
      return NULL;
    }

  css3->angle = 0.0;
  css3->n     = 0;
  css3->stop  = NULL;

  return css3;
}

/* this can only be done once */

extern int css3_stops_alloc(css3_t* css3,int n)
{
  css3_stop_t* stop;

  if (n<1) return 1;

  if (css3->n > 0) return 1;

  if ((stop = malloc(n*sizeof(css3_stop_t))) == NULL) 
    {
      btrace("failed memory allocation");
      return 1;
    }
      
  css3->n     = n;
  css3->stop  = stop;

  return 0;
}

extern void css3_destroy(css3_t* css3)
{
  if (css3->n) free(css3->stop);
  free(css3);
}

