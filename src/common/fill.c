/*
  fill.h
  
  fills for gimpcpt

  (c) J.J.Green 2001,2004
  $Id: fill.h,v 1.5 2004/03/16 01:26:45 jjg Exp $
*/

#include "fill.h"

extern int fill_eq(fill_t a,fill_t b)
{
  if (a.type != b.type) return 0;

  switch (a.type)
    {
    case empty : return 1;
    case grey :
      return (a.u.grey == b.u.grey);
    case hatch :
      return ((a.u.hatch.sign == b.u.hatch.sign) &&
	      (a.u.hatch.dpi == b.u.hatch.dpi) &&
	      (a.u.hatch.n == b.u.hatch.n));
    case file : /* fixme */
      return 0;
    case colour :
      return colour_rgb_dist(a.u.colour,b.u.colour,rgb)<1e-8;
    }
}
