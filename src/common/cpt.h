/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on theml

  (c) J.J.Green 2001
  $Id: cpt.h,v 1.2 2001/05/21 00:28:06 jjg Exp $
*/

#ifndef CPT_H
#define CPT_H

#include "colour.h"

typedef struct cpt_sample_t
{
    double val;
    rgb_t  rgb;
} cpt_sample_t;

typedef struct cpt_seg_t
{
    struct cpt_seg_t *lseg,*rseg;
    cpt_sample_t      lsmp, rsmp;
} cpt_seg_t;

typedef struct cpt_t
{
    char      *name;    
    rgb_t     fg,bg,nan;
    cpt_seg_t *segment;
} cpt_t;

extern cpt_t* cpt_new();
extern int cpt_prepend(cpt_seg_t*,cpt_t* cpt);
extern int cpt_append(cpt_seg_t*,cpt_t* cpt);
extern int cpt_write(char* name,cpt_t* cpt);
extern void cpt_destroy(cpt_t*);

extern cpt_seg_t* cpt_seg_new();
extern void cpt_seg_destroy(cpt_seg_t*);

#endif



