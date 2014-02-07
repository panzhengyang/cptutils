/*
  grd5.h

  structure holding a parsed grd5 (Photoshop gradient)

  Copyright (c) J.J. Green 2014
  $Id$
*/

#include <stdlib.h>
#include "grd5.h"

extern void grd5_grad_destroy(grd5_grad_t *grad)
{
  free(grad->colour.stops);
  free(grad->transp.stops);
  grd5_string_destroy(grad->title);
}


extern void grd5_destroy(grd5_t *grd5)
{
  int i;

  for (i=0 ; i < grd5->n ; i++)
    grd5_grad_destroy(grd5->gradients+i);
  free(grd5->gradients);

  free(grd5);
}
