/*
  grd5.h

  structure holding a parsed grd5 (Photoshop gradient)

  Copyright (c) J.J. Green 2014
  $Id$
*/

#ifndef GRD5_H
#define GRD5_H

#include <stdint.h>
#include "grd5string.h"

#define GRD5_STOP_RGB  1
#define GRD5_STOP_HSB  2
#define GRD5_STOP_LAB  3
#define GRD5_STOP_GRSC 4
#define GRD5_STOP_FRGC 5
#define GRD5_STOP_BCKC 6

typedef struct
{
  double   Opct;
  uint32_t Lctn;
  uint32_t Mdpn;
} grd5_transp_stop_t;

typedef struct
{
  double Rd, Grn, Bl;
} grd5_rgb_t;

typedef struct
{
  double H, Strt, Brgh;
} grd5_hsb_t;

typedef struct
{
  double Lmnc, A, B;
} grd5_lab_t;

typedef struct
{
  double Gry;
} grd5_grsc_t;

typedef struct
{
  int type;
  union {
    grd5_rgb_t  rgb;
    grd5_hsb_t  hsb;
    grd5_lab_t  lab;
    grd5_grsc_t grsc;
  } u;
  uint32_t Lctn;
  uint32_t Mdpn;
} grd5_colour_stop_t;

typedef struct
{
  double interp;
  grd5_string_t *title;
  struct {
    uint32_t n;
    grd5_colour_stop_t *stops;
  } colour;
  struct {
    uint32_t n;
    grd5_transp_stop_t *stops;
  } transp;
} grd5_grad_t;

typedef struct
{
  uint32_t n;
  grd5_grad_t* gradients;
} grd5_t;

extern void grd5_destroy(grd5_t *grd5);

#endif
