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

  $Id: dp-simplify.c,v 1.1 2004/02/23 00:19:36 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "dp-simplify.h"

/*
  simple vector functions

  these would be best implemented as calls to appropriate
  functions in a library (the GSL say), but we need so little
  it seems a shame to intoduce the dependency
*/

static double dot(vertex_t u, vertex_t v)
{
  int i;
  double sum = 0.0;

  for (i=0 ; i<VDIM ; i++)  sum += u.x[i]*v.x[i];

  return sum;
}

static double norm2(vertex_t u)
{
  return dot(u,u);
}

static vertex_t vdiff(vertex_t u, vertex_t v)
{
  int i;
  vertex_t w;

  for (i=0 ; i<VDIM ; i++)  w.x[i] = u.x[i] - v.x[i];

  return w;
}

static vertex_t vadd(vertex_t u, vertex_t v)
{
  int i;
  vertex_t w;

  for (i=0 ; i<VDIM ; i++)  w.x[i] = u.x[i] + v.x[i];

  return w;
}

static vertex_t vsm(double l, vertex_t v)
{
  int i;
  vertex_t w;

  for (i=0 ; i<VDIM ; i++)  w.x[i] = l*v.x[i];

  return w;
}

static double d2(vertex_t u, vertex_t v)
{
  vertex_t w;

  w = vdiff(u,v);

  return norm2(vdiff(u,v));
}

/*
  poly_simplify():
    Input:  tol = approximation tolerance
            p   = polyline array of vertex points
    Output: q   = simplified polyline vertices
*/

static int simplify_dp(double,vertex_t*,int,int,int*);

extern int poly_simplify(double t,vertex_t* pv,int n,int* mk)
{
  int       i,k,l;
  vertex_t  v[n];
  float     t2 = t*t;

  /* clear errno to chck for math errors */

  errno = 0;

  /* vertex reduction within tolerance of prior vertex cluster */

  v[0] = pv[0];
  
  for (i=k=1,l=0 ; i<n ; i++) 
    {
      if (d2(pv[i], pv[l]) < t2) continue;
      v[k++] = pv[i];
      l = i;
    }

  if (l < n-1) v[k++] = pv[n-1];
  
  /* Douglas-Peucker polyline simplification */

  for (i=0 ; i<k ; i++) 
    mk[i] = 0;

  mk[0] = mk[k-1] = 1;

  if (simplify_dp(t2,v,0,k-1,mk) != 0) return 1;
  
  /* check for math errors */

  if (errno)
    {
      fprintf(stderr,"error in simplification : %s\n",strerror(errno));
      return 1;
    }

  return 0;
}

/*
  simplify_dp():
  
  This is the Douglas-Peucker recursive simplification routine
  It just marks vertices that are part of the simplified polyline
  for approximating the polyline subchain v[j] to v[k].

    Input:  t2   = tolerance squared
            v[]  = polyline array of vertex points
            j,k  = indices for the subchain v[j] to v[k]
    Output: mk[] = array of markers matching vertex array v[]
*/

static int simplify_dp(double t2,vertex_t* v,int j,int k,int* mk)
{
  double   maxd2 = 0;		     /* distance squared of farthest vertex */
  int      maxi  = j;		     /* index of vertex farthest from S */
  vertex_t u     = vdiff(v[k],v[j]); /* segment direction vector */
  double   cu    = dot(u,u);         /* segment length squared */
  int      i;

  /*
    test each vertex v[i] for max distance from S
    compute using the Feb 2001 Algorithm's dist_Point_to_Segment()
    Note: this works in any dimension (2D, 3D, ...)
  */
    
  vertex_t  w,p;
  double    b,cw,dv2;
 
  if (k <= j+1) return 0;

  for (i=j+1 ; i<k ; i++)
    {
      /* compute distance squared */

      w  = vdiff(v[i],v[j]);
      cw = dot(w,u);

      if (cw <= 0)
	dv2 = d2(v[i],v[j]);
      else if (cu <= cw)
	dv2 = d2(v[i],v[k]);
      else 
	{
	  b   = cw/cu;
	  p   = vadd(v[j],vsm(b,u));
	  dv2 = d2(v[i],p);
        }

      /* test with current max distance squared */

      if (dv2 <= maxd2) continue;

      /* v[i] is a new max vertex */

      maxi  = i;
      maxd2 = dv2;
    }

  /* 
     error is worse than the tolerance 
     split the polyline at the farthest vertex from S 
  */

  if (maxd2 > t2)
    {
      /* mark v[maxi] for the simplified polyline */
      
      mk[maxi] = 1;

      /* recursively simplify the two subpolylines at v[maxi] */
      
      simplify_dp(t2,v,j,maxi,mk);  /* polyline v[j] to v[maxi] */
      simplify_dp(t2,v,maxi,k,mk);  /* polyline v[maxi] to v[k] */
    }

  /* else the approximation is OK, so ignore intermediate vertices */

  return 0;
}

 


