/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on theml

  (c) J.J.Green 2001
  $Id: cpt.c,v 1.10 2004/03/18 02:27:18 jjg Exp jjg $
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
  double tol = 1e-6;

  left=cpt->segment;

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
    
    if ((seg = cpt->segment) == NULL) return;

    while (seg) 
    {
	next = seg->rseg;
	cpt_seg_destroy(seg);
	seg = next;
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



