/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on them

  (c) J.J.Green 2001,2004
  $Id: cpt.h,v 1.5 2004/02/24 18:37:27 jjg Exp jjg $
*/

#ifndef CPT_H
#define CPT_H

#include "colour.h"

typedef struct cpt_sample_t
{
  double val;
  colour_t col;
} cpt_sample_t;

typedef struct cpt_seg_t
{ 
    struct cpt_seg_t *lseg,*rseg;
    cpt_sample_t      lsmp, rsmp;
} cpt_seg_t;

#define CPT_NAME_LEN 128

typedef struct cpt_t
{
  char       name[CPT_NAME_LEN];
  model_t    model;
  colour_t   fg,bg,nan;
  cpt_seg_t *segment;
} cpt_t;

extern cpt_t* cpt_new();
extern void cpt_destroy(cpt_t*);

extern int cpt_prepend(cpt_seg_t*,cpt_t*);
extern int cpt_append(cpt_seg_t*,cpt_t*);

extern cpt_seg_t* cpt_seg_new();
extern void cpt_seg_destroy(cpt_seg_t*);

extern cpt_seg_t* cpt_segment(cpt_t*,int);
extern int cpt_nseg(cpt_t*);
extern int cpt_npc(cpt_t*,int*);

#endif



