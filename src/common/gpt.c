/*
  gpt.h

  structures for gnuplot colour maps, and a couple of
  low-level manipulations

  J.J.Green 2010
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gpt.h"

extern gpt_t* gpt_new(void)
{
  gpt_t* gpt;

  if ((gpt = malloc(sizeof(gpt_t))) == NULL) return NULL;

  gpt->n    = 0;
  gpt->stop = NULL;

  return gpt;
}

/* this can only be done once */

extern int gpt_stops_alloc(gpt_t* gpt,int n)
{
  gpt_stop_t* stop;

  if ( n<1 ) return 1;

  if ( gpt->n > 0) return 1;

  if ((stop = malloc(n*sizeof(gpt_stop_t))) == NULL) 
    return 1;
      
  gpt->n    = n;
  gpt->stop = stop;

  return 0;
}

extern void gpt_destroy(gpt_t* gpt)
{
  if (gpt->n) free(gpt->stop);
  free(gpt);
}

