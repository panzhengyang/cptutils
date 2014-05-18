/*
  grd5.h

  structure holding a parsed grd5 (Photoshop gradient)

  Copyright (c) J.J. Green 2014
  $Id$
*/

#include <stdlib.h>
#include "grd5.h"

extern void grd5_grad_custom_destroy(grd5_grad_custom_t *gradc)
{
  int n = gradc->colour.n;

  if (n > 0)
    {
      int i;

      grd5_colour_stop_t *stops = gradc->colour.stops;

      for (i=0 ; i<n ; i++)
	{
	  if (stops[i].type == GRD5_MODEL_BOOK)
	    {
	      grd5_book_t *book = &(stops[i].u.book);

	      grd5_string_destroy(book->Bk);
	      grd5_string_destroy(book->Nm);
	      grd5_string_destroy(book->bookKey);
	    }
	}

      free(gradc->colour.stops);
    }
  if (gradc->transp.n > 0) free(gradc->transp.stops);
}

extern void grd5_grad_noise_destroy(grd5_grad_noise_t *gradn)
{
  if (gradn->min.n > 0) free(gradn->min.vals);
  if (gradn->max.n > 0) free(gradn->max.vals);
}

extern void grd5_grad_destroy(grd5_grad_t *grad)
{
  grd5_string_destroy(grad->title);
  switch (grad->type)
    {
    case GRD5_GRAD_CUSTOM:
      grd5_grad_custom_destroy(&(grad->u.custom));
      break;
    case GRD5_GRAD_NOISE:
      grd5_grad_noise_destroy(&(grad->u.noise));
      break;
     }
}

extern void grd5_destroy(grd5_t *grd5)
{
  if (grd5->n > 0)
    {
      int i;

      for (i=0 ; i < grd5->n ; i++)
	grd5_grad_destroy(grd5->gradients+i);

      free(grd5->gradients);
    }

  free(grd5);
}

extern int grd5_model(grd5_string_t *str)
{
  if (str->len < 1) 
    return GRD5_MODEL_UNKNOWN;

  int model = GRD5_MODEL_UNKNOWN;

  switch (str->content[0])
    {
    case 'R':
      if (grd5_string_matches(str, "RGBC"))
	model = GRD5_MODEL_RGB;
      break;
    case 'H':
      /*
	Oddly, HSBl seems only to be used in noise gradients; not sure
	if it should be interpreted as HSB
      */
      if (grd5_string_matches(str, "HSBC") || grd5_string_matches(str, "HSBl"))
      model = GRD5_MODEL_HSB;
      break;
    case 'L':
      if (grd5_string_matches(str, "LbCl"))
	model = GRD5_MODEL_LAB;
      break;
    case 'C':
      if (grd5_string_matches(str, "CMYC"))
	model = GRD5_MODEL_CMYC;
      break;
    case 'G':
      if (grd5_string_matches(str, "Grsc"))
	model = GRD5_MODEL_GRSC;
      break;
    case 'F':
      if (grd5_string_matches(str, "FrgC"))
	model = GRD5_MODEL_FRGC;
      break;
    case 'B':
      if (grd5_string_matches(str, "BckC"))
	model = GRD5_MODEL_BCKC;
      else if (grd5_string_matches(str, "BkCl"))
	model = GRD5_MODEL_BOOK;
      break;
    }

  return model;
}
