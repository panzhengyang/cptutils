/*
  fill.h
  
  fills for cpt stuctures

  (c) J.J.Green 2001,2004
  $Id: fill.c,v 1.3 2004/04/12 23:42:08 jjg Exp jjg $
*/

#include <stdio.h>
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

  fprintf(stderr,"no such fill type\n");

  return 1;
}

extern int fill_interpolate(double z,fill_t a,fill_t b,model_t model,fill_t *f)
{
  filltype_t type;

  if (a.type != b.type) 
    return 1;

  type = a.type;

  f->type = type;

  switch (type)
    {
    case empty : 
      break;

    case grey :
      f->u.grey = (a.u.grey*(1-z) + b.u.grey);
      break;

    case hatch :
    case file :
      *f = a;
      break;

    case colour :
      if (colour_interpolate(z,a.u.colour,b.u.colour,model,&(f->u.colour)) != 0)
	{
	  fprintf(stderr,"failed to interpolate colour\n");
	  return 1;
	}

      break;
    }

  return 0;
}

/*
  convert a fill to an rgb triplet -- often a recieving 
  program will only accept a colour.
*/

extern int fill_rgb(fill_t fill,model_t model,rgb_t *prgb)
{
  switch (fill.type)
    {
    case colour:
      switch (model)
	{
	  double rgbD[3];

	case hsv:
	  hsv_to_rgbD(fill.u.colour.hsv,rgbD);
	  rgbD_to_rgb(rgbD,prgb);
	  break;

	case rgb:
	  *prgb = fill.u.colour.rgb;
	  break;

	default:
	  fprintf(stderr,"bad colour model (%i)\n",model); 
	  return 1;
	}
      break;

    case grey:
    case hatch:
    case file:
    case empty:

      fprintf(stderr,"fill type not yet implemeted\n"); 
      return 1;

    default:
      fprintf(stderr,"bad fill type (%i)\n",fill.type); 
      return 1;
    }

  return 0;
}
