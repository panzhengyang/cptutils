/*
  grd3.c

  paintshop pro gradient structures
  2005 (c) J.J. Green
*/

#include "grd3.h"

unsigned char grd3magic[] = "8BGR";

extern grd3_t* grd3_new(void)
{
  grd3_t* grd3;

  if ((grd3 = malloc(sizeof(grd3_t))) == NULL)
    return NULL;

  grd3->name    = NULL;

  /* 
     all grd3 files seem to have this version (and grd3 x will
     not load a file which does not have it), but the caller
     can modify it
  */

  grd3->ver[0]  = 3;
  grd3->ver[1]  = 1;

  grd3->rgb.n   = 0;
  grd3->rgb.seg = NULL;

  grd3->op.n    = 0;
  grd3->op.seg  = NULL;

  return grd3;
}

extern void grd3_destroy(grd3_t* grd3)
{
  free(grd3->name);
  free(grd3->rgb.seg);
  free(grd3->op.seg);

  free(grd3);
}
