/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on theml

  (c) J.J.Green 2001
  $Id: cpt.c,v 1.13 2004/04/12 23:42:00 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "colour.h"
#include "cpt.h"

extern cpt_seg_t* cpt_segment(cpt_t* cpt,int n)
{
  cpt_seg_t *seg;

  seg = cpt->segment;

  while (n--)
    {
      if ((seg = seg->rseg) == NULL) 
	return NULL; 
    }

  return seg;
}

extern int cpt_nseg(cpt_t* cpt)
{
  int n = 0;
  cpt_seg_t *seg;

  for (seg = cpt->segment ; seg ; seg = seg->rseg)  n++;

  return n;
}

extern int cpt_npc(cpt_t* cpt,int *segos)
{
  int i,n;
  cpt_seg_t *left,*right;

  left = cpt->segment;

  if (! left) return 0;

  segos[0] = 0;

  right = left->rseg;

  if (! right) return 1;

  n = 1;
  i = 0;

  while (right)
    {    
      if (! fill_eq(left->rsmp.fill,right->lsmp.fill))
	{
	  segos[n] = i;
	  n++;
	}

      left  = right;
      right = left->rseg;
      i++;
    }

  return n;
}

extern int cpt_zrange(cpt_t* cpt,double* z)
{
  cpt_seg_t *seg;

  seg = cpt->segment;

  if (!seg) return 1;

  z[0] = seg->lsmp.val;

  while (seg->rseg) 
    seg = seg->rseg;

  z[1] = seg->rsmp.val;

  return 0;
}

extern int cpt_zfill(cpt_t* cpt,double z,fill_t* fill)
{
  cpt_seg_t *seg;
  model_t    model;

  seg   = cpt->segment;
  model = cpt->model;

  if (!seg) return 1;

  if (z < seg->lsmp.val)
    {
      *fill = cpt->bg;
      return 0;
    }

  do
    {
      double z0,z1;

      z0 = seg->lsmp.val;
      z1 = seg->rsmp.val;

      if ((z0 <= z) && (z <= z1))
	{
	  double zp;

	  zp = (z-z0)/(z1-z0);

	  if (fill_interpolate(zp,seg->lsmp.fill,seg->rsmp.fill,model,fill) != 0)
	    {
	      fprintf(stderr,"fill interpolation failed\n");
	      return 1;
	    }

	  return 0;
	}

      if (seg->rseg == NULL)
	{
	  if (z1<=z)
	    {
	      *fill = cpt->fg;
	      return 0;
	    }
	}
    }
  while ((seg = seg->rseg) != NULL);

  fprintf(stderr,"odd error - attempt to sample empty z-slice?\n");

  return 1;
}

extern cpt_t* cpt_new(void)
{
    cpt_t* cpt;

    if ((cpt = malloc(sizeof(cpt_t))) == NULL)
      return NULL;

    cpt->segment = NULL;

    return cpt;
}

extern int cpt_prepend(cpt_seg_t* seg,cpt_t* cpt)
{
    cpt_seg_t* s;

    s = cpt->segment;

    seg->lseg = NULL;
    seg->rseg = s;

    if (s) s->lseg = seg;

    cpt->segment = seg;

    return 0;
}

extern int cpt_append(cpt_seg_t* seg,cpt_t* cpt)
{
    cpt_seg_t* s;

    s = cpt->segment;

    if (!s) return cpt_prepend(seg,cpt);
 
    while (s->rseg) s = s->rseg;
 
    s->rseg = seg;
    seg->lseg = s;
    seg->rseg = NULL;
 
    return 0;
}

extern void cpt_destroy(cpt_t* cpt)
{
    cpt_seg_t *seg, *next;

    if (!cpt) return;
    
    if ((seg = cpt->segment) != NULL)
      {
	while (seg) 
	  {
	    next = seg->rseg;
	    cpt_seg_destroy(seg);
	    seg = next;
	  }
      }
 
    free(cpt);

    return;
}

extern cpt_seg_t* cpt_seg_new(void)
{
    cpt_seg_t *seg;

    if ((seg = malloc(sizeof(cpt_seg_t))) == NULL) 
	return NULL;

    seg->lseg   = NULL;
    seg->rseg   = NULL;
    seg->annote = none;

    return seg;
}

extern void cpt_seg_destroy(cpt_seg_t* seg)
{
    free(seg);
}



