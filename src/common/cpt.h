/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on them

  (c) J.J.Green 2001, 2004
*/

#ifndef CPT_H
#define CPT_H

#include "fill.h"

typedef struct cpt_sample_t
{
  double val;
  fill_t fill;
} cpt_sample_t;

typedef enum {both,lower,upper,none} annote_t;

typedef struct cpt_seg_t
{ 
  struct cpt_seg_t *lseg,*rseg;
  cpt_sample_t      lsmp, rsmp;
  annote_t          annote;
  char             *label;
} cpt_seg_t;

typedef struct cpt_t
{
  char      *name;
  model_t    model;
  fill_t     fg,bg,nan;
  cpt_seg_t *segment;
} cpt_t;

extern cpt_t* cpt_new();
extern void cpt_destroy(cpt_t*);

extern int cpt_prepend(cpt_seg_t*,cpt_t*);
extern int cpt_append(cpt_seg_t*,cpt_t*);

extern cpt_seg_t* cpt_pop(cpt_t*);
extern cpt_seg_t* cpt_shift(cpt_t*);

extern cpt_seg_t* cpt_seg_new();
extern void cpt_seg_destroy(cpt_seg_t*);

extern cpt_seg_t* cpt_segment(cpt_t*,int);
extern int cpt_nseg(cpt_t*);
extern int cpt_npc(cpt_t*,int*);
extern int cpt_increasing(cpt_t*);

extern int cpt_zrange(cpt_t*,double*);
extern int cpt_zfill(cpt_t*,double,fill_t*);

#endif



