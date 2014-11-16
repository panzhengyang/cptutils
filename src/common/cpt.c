/*
  cpt.c

  A struct to hold a GMT cpt file, and some operations 
  on them

  (c) J.J.Green 2001
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "colour.h"
#include "cpt.h"
#include "btrace.h"

extern cpt_seg_t* cpt_segment(cpt_t* cpt, int n)
{
  cpt_seg_t *seg;

  seg = cpt->segment;

  while (n--)
    {
      if ((seg = seg->rseg) == NULL) 
	{
	  btrace("no right-link for segment %i", n);
	  return NULL;
	} 
    }

  return seg;
}

/*
  count the number of segments
*/

extern int cpt_nseg(cpt_t* cpt)
{
  int n = 0;
  cpt_seg_t *seg;

  for (seg = cpt->segment ; seg ; seg = seg->rseg)  n++;

  return n;
}

/*
  get the offsets of the continuous pieces 
  in an array of seg, so if the segs are

    [blue red], [red red], [red blue], [pink pink],...

  the the segos array would start [0,3,...]. 

  the function returns the number of pieces.
*/

#define DEBUG

extern int cpt_npc(cpt_t* cpt,int *segos)
{
  int i,n;
  cpt_seg_t *left,*right;

  left = cpt->segment;

  if (! left) return 0;

  segos[0] = 0;

  right = left->rseg;

  if (! right) return 1;

  for (n = 1, i = 1 ; right ; i++)
    {    
      if (! fill_eq(left->rsmp.fill,right->lsmp.fill))
	{
	  segos[n] = i;
	  n++;
	}

      left  = right;
      right = left->rseg;
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

/* 
   returns 1 if the cpt z-range is increasing
   and zero otherwise.  The input is expected
   to have at least one segment.
*/

extern int cpt_increasing(cpt_t* cpt)
{
  double z[2] = {0};

  cpt_zrange(cpt, z);

  return z[0] < z[1];
}

extern int cpt_zfill(cpt_t* cpt, double z, fill_t* fill)
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
      double z0, z1;

      z0 = seg->lsmp.val;
      z1 = seg->rsmp.val;

      if ((z0 <= z) && (z <= z1))
	{
	  double zp;

	  zp = (z-z0)/(z1-z0);

	  if (fill_interpolate(zp,
			       seg->lsmp.fill,
			       seg->rsmp.fill,
			       model,
			       fill) != 0)
	    {
	      btrace("fill interpolation failed");
	      return 1;
	    }

	  return 0;
	}

      if (seg->rseg == NULL)
	{
	  if (z1 <= z)
	    {
	      *fill = cpt->fg;
	      return 0;
	    }
	}
    }
  while ((seg = seg->rseg) != NULL);

  btrace("odd error - attempt to sample empty z-slice?");

  return 1;
}

extern cpt_t* cpt_new(void)
{
  cpt_t* cpt;
  
  if ((cpt = malloc(sizeof(cpt_t))) == NULL)
    return NULL;
  
  cpt->name     = NULL;
  cpt->segment  = NULL;
  cpt->model    = model_rgb;
  cpt->fg.type  = fill_empty;
  cpt->bg.type  = fill_empty;
  cpt->nan.type = fill_empty;
  
  return cpt;
}

/* add a segment to the start (leftmost) */

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

/* add a segment to the end (rightmost) */

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

/* remove first segment (and return it) */

extern cpt_seg_t* cpt_pop(cpt_t* cpt)
{
  cpt_seg_t* s1 = cpt->segment;

  if (s1)
    {
      cpt_seg_t* s2 = s1->rseg;

      if (s2) s2->lseg = NULL;
      
      cpt->segment = s2;

      s1->rseg = NULL;
      s1->lseg = NULL;

      return s1;
    }

  return NULL;
}

/* remove last segment (and return it) */

extern cpt_seg_t* cpt_shift(cpt_t* cpt)
{
  cpt_seg_t* s1;

  s1 = cpt->segment;
  
  /* no segment case */

  if (!s1) return NULL;

  /* single segment case */

  if (! s1->rseg)
    {
      cpt->segment = NULL;

      s1->lseg = NULL;
      s1->rseg = NULL;

      return s1;
    }

  /* multi segment case */

  while (s1->rseg->rseg) s1 = s1->rseg;
  
  cpt_seg_t* s2 = s1->rseg;
  
  s1->rseg = NULL;
  
  s2->lseg = NULL;
  s2->rseg = NULL;
  
  return s2;
}

extern void cpt_destroy(cpt_t* cpt)
{
  if (cpt)
    {
      if (cpt->name)
	free(cpt->name);

      cpt_seg_t *seg;

      if ((seg = cpt->segment) != NULL)
	{
	  cpt_seg_t *next;

	  while (seg) 
	    {
	      next = seg->rseg;
	      cpt_seg_destroy(seg);
	      seg = next;
	    }
	}
 
      free(cpt);
    }

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
  seg->label  = NULL;
  
  return seg;
}

extern void cpt_seg_destroy(cpt_seg_t* seg)
{
  if (seg->label) free(seg->label);
  free(seg);
}




