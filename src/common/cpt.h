/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on them

  (c) J.J.Green 2001,2004
  $Id: cpt.h,v 1.2 2004/02/11 00:58:59 jjg Exp jjg $
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

typedef enum {rgb,hsv} model_t;

#define CPT_NAME_LEN 128

typedef struct cpt_t
{
  char      name[CPT_NAME_LEN];
  model_t   model;
  rgb_t     fg,bg,nan;
  cpt_seg_t *segment;
} cpt_t;

extern cpt_t* cpt_new();
extern void cpt_destroy(cpt_t*);

extern int cpt_prepend(cpt_seg_t*,cpt_t*);
extern int cpt_append(cpt_seg_t*,cpt_t*);
extern int cpt_write(char*,cpt_t*);
extern int cpt_read(char*,cpt_t*);

extern cpt_seg_t* cpt_seg_new();
extern void cpt_seg_destroy(cpt_seg_t*);

#endif



