/*
  fill.h
  
  fills for cpt stuctures

  (c) J.J.Green 2001,2004
*/

#include <stdio.h>
#include "fill.h"
#include "btrace.h"

extern int fill_eq(fill_t a, fill_t b)
{
  if (a.type != b.type) return 0;

  switch (a.type)
    {
    case fill_empty : return 1;
    case fill_grey :
      return (a.u.grey == b.u.grey);
    case fill_hatch :
      return ((a.u.hatch.sign == b.u.hatch.sign) &&
	      (a.u.hatch.dpi == b.u.hatch.dpi) &&
	      (a.u.hatch.n == b.u.hatch.n));
    case fill_file : /* fixme */
      return 0;
    case fill_colour :
      return colour_rgb_dist(a.u.colour, b.u.colour, model_rgb) < 1e-8;
    }

  btrace("no such fill type");

  return 1;
}

extern int fill_interpolate(double z, fill_t a, fill_t b, 
			    model_t model, fill_t *f)
{
  filltype_t type;

  if (a.type != b.type) 
    return 1;

  type = a.type;

  f->type = type;

  switch (type)
    {
    case fill_empty : 
      break;

    case fill_grey :
      f->u.grey = (a.u.grey*(1-z) + b.u.grey*z);
      break;

    case fill_hatch :
    case fill_file :
      *f = a;
      break;

    case fill_colour :
      if (colour_interpolate(z, a.u.colour, b.u.colour, 
			     model, &(f->u.colour)) != 0)
	{
	  btrace("failed to interpolate colour");
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
    case fill_colour:
      switch (model)
	{
	  double rgbD[3];

	case model_hsv:
	  hsv_to_rgbD(fill.u.colour.hsv,rgbD);
	  rgbD_to_rgb(rgbD,prgb);
	  break;

	case model_rgb:
	  *prgb = fill.u.colour.rgb;
	  break;

	default:
	  btrace("bad colour model (%i)",model); 
	  return 1;
	}
      break;

    case fill_grey:
    case fill_hatch:
    case fill_file:
    case fill_empty:

      btrace("fill type not yet implemeted"); 
      return 1;

    default:
      btrace("bad fill type (%i)",fill.type); 
      return 1;
    }

  return 0;
}
