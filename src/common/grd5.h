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

#define GRD5_MODEL_UNKNOWN 0
#define GRD5_MODEL_RGB     1
#define GRD5_MODEL_HSB     2
#define GRD5_MODEL_LAB     3
#define GRD5_MODEL_CMYC    4
#define GRD5_MODEL_GRSC    5
#define GRD5_MODEL_FRGC    6
#define GRD5_MODEL_BCKC    7

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
  double Cyn, Mgnt, Ylw, Blck;
} grd5_cmyc_t;

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
    grd5_cmyc_t cmyc;
    grd5_grsc_t grsc;
  } u;
  uint32_t Lctn;
  uint32_t Mdpn;
} grd5_colour_stop_t;

#define GRD5_GRAD_CUSTOM 1
#define GRD5_GRAD_NOISE  2

typedef struct
{
  uint32_t n;
  uint32_t *vals;
} grd5_extremum_t;

typedef struct
{
  bool show_transparency;
  bool vector_colour;
  uint32_t seed;
  uint32_t smoothness;
  int model;
  grd5_extremum_t min, max;
} grd5_grad_noise_t;

typedef struct
{
  double interp;
  struct {
    uint32_t n;
    grd5_colour_stop_t *stops;
  } colour;
  struct {
    uint32_t n;
    grd5_transp_stop_t *stops;
  } transp;
} grd5_grad_custom_t;

typedef struct
{
  int type;
  grd5_string_t *title;
  union
  {
    grd5_grad_custom_t custom;
    grd5_grad_noise_t noise;
  } u;
} grd5_grad_t;

typedef struct
{
  uint32_t n;
  grd5_grad_t* gradients;
} grd5_t;

extern int grd5_model(grd5_string_t *str);
extern void grd5_destroy(grd5_t *grd5);

#endif
