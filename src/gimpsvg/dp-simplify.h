/*
  dp-simplify.c

  Douglas-Peucker polyline simplification
  J.J.Green 2003

  following code derived from dp.cc, a simple recursive
  implementation by softsurfer -- cheers mate!

  Ive made some cosmetic changes, errors intoduced are
  probably my own

  jjg 2004

  original notice follows

  ----
  Copyright 2002, softSurfer (www.softsurfer.com)
  This code may be freely used and modified for any purpose
  providing that this copyright notice is included with it.
  SoftSurfer makes no warranty for this code, and cannot be held
  liable for any real or imagined damage resulting from its use.
  Users of this code must verify correctness for their application.
  ---

  $Id: dp-simplify.c,v 1.2 2004/01/26 00:25:19 jjg Exp $
*/

#ifndef DPSIMPLIFY_H 
#define DPSIMPLIFY_H 

/*
  the dimension of the verticies
*/

#define VDIM 3

typedef struct
{
  double x[VDIM];
} vertex_t;

extern int poly_simplify(double,vertex_t*,int,int*);

#endif
