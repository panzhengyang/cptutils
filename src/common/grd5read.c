/*
  grd5read.c
  Copyright (c) J.J. Green 2014
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "grd5read.h"
#include "grd5type.h"
#include "grd5string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* FIXME : this is linux-specific : handle *BSD too */

#ifdef HAVE_ENDIAN_H
#include <endian.h>
#endif

static int grd5_stream(FILE* stream, grd5_t* grd5);

extern int grd5_read(const char* file, grd5_t** pgrd5)
{
  int err = GRD5_READ_BUG;

  grd5_t *grd5 = malloc(sizeof(grd5_t));
  if (grd5 == NULL) return GRD5_READ_MALLOC;

  if (file)
    {
      FILE *stream;
      
      if ((stream = fopen(file, "r")) == NULL)
	return GRD5_READ_FOPEN;

      err = grd5_stream(stream, grd5);

      fclose(stream);
    }
  else
    err = grd5_stream(stdin, grd5);

  if (err == GRD5_READ_OK)
    *pgrd5 = grd5;
  else
    grd5_destroy(grd5);

  return err;
}

static int parse_uint16(FILE *stream, uint16_t *i)
{
  uint16_t buf;

  if (fread(&buf, 2, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *i = be16toh(buf);

  return GRD5_READ_OK;
}

static int parse_uint32(FILE *stream, uint32_t *i)
{
  uint32_t buf;

  if (fread(&buf, 4, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *i = be32toh(buf);

  return GRD5_READ_OK;
}

static int parse_type(FILE *stream, int *type)
{
  char buf[4];

  if (fread(&buf, 1, 4, stream) != 4)
    return GRD5_READ_FREAD;

  *type = grd5_type(buf);

  return GRD5_READ_OK;
}

typedef enum { 
  grd5_string_typename, 
  grd5_string_ucs2 
} grd5_string_type_t;

static grd5_string_t* parse_grd5_string(FILE *stream, 
					grd5_string_type_t type,
					int *perr)
{
  uint32_t len;

  if ((*perr = parse_uint32(stream, &len)) != GRD5_READ_OK)
    return NULL;

  switch (type)
    {
    case grd5_string_typename :
      if (len == 0) len = 4;
      break;
    case grd5_string_ucs2 :
      len *= 2;
      break;
    default:
      *perr = GRD5_READ_BUG;
    }

  char *content;

  if ((content = malloc(len)) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      return NULL;
    }

  if (fread(content, 1, len, stream) != len)
    {
      *perr = GRD5_READ_FREAD;
      goto cleanup_content;
    }

  grd5_string_t* gstr;

  if ((gstr = malloc(sizeof(grd5_string_t))) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      goto cleanup_content;
    }

  gstr->len     = len;
  gstr->content = content;

  return gstr;

 cleanup_content:
  
  free(content);
  
  return NULL;
}

static grd5_string_t* parse_typename(FILE *stream, int *perr)
{
  return parse_grd5_string(stream, grd5_string_typename, perr);
}

static grd5_string_t* parse_ucs2(FILE *stream, int *perr)
{
  return parse_grd5_string(stream, grd5_string_ucs2, perr);
}

static bool typename_matches(grd5_string_t* typename, const char* expected)
{
  return grd5_string_matches(typename, expected);
}

static int parse_named_type(FILE *stream, 
			    const char* expected_name, 
			    int expected_type)
{ 
  grd5_string_t *typename;
  int err;

  if ((typename = parse_typename(stream, &err)) == NULL)
    return err;
  else
    {
      bool matches = typename_matches(typename, expected_name);
      if (!matches)
	fprintf(stderr, "expected type %s, read %*s\n",
		expected_name, typename->len, typename->content);
      grd5_string_destroy(typename);
      if (!matches) return GRD5_READ_PARSE;
    }

  int type;

  if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
    return err;

  if (type == TYPE_UNKNOWN)
    fprintf(stderr, "unknown type\n");

  if (type != expected_type)
    return GRD5_READ_PARSE;

  return GRD5_READ_OK;
}

static int parse_double_data(FILE *stream, double *pval)
{
  uint64_t val;

  if (fread(&val, 8, 1, stream) != 1) return GRD5_READ_FREAD;

  val = be64toh(val);
  
  /*
    the following #pragmas supresses the warning "dereferencing 
    type-punned pointer will break strict-aliasing rules" since
    that is exactly what we want to do
  */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

  *pval = *(double*)(&val);

#pragma GCC diagnostic pop

  return GRD5_READ_OK;
}

static int parse_untf(FILE *stream, 
		      const char* expected_name, 
		      const char* expected_unit, 
		      double *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_UNTF)) != GRD5_READ_OK)
    return err;
  
  char unit[4];

  if (fread(unit, 1, 4, stream) != 4)
    return GRD5_READ_FREAD;

  if (strncmp(unit, expected_unit, 4) != 0)
    {
      fprintf(stderr, "expected unit %s, read %4s\n", expected_unit, unit);
      return GRD5_READ_PARSE;
    }

  return parse_double_data(stream, pval);
}

static int parse_double(FILE *stream, const char* expected_name, double *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_DOUBLE)) != GRD5_READ_OK)
    return err;

  return parse_double_data(stream, pval);
}

static int parse_enum(FILE *stream, 
		      const char* expected_typename,
		      grd5_string_t** name,
		      grd5_string_t** subname)
{
  int err;

  if ((err = parse_named_type(stream, expected_typename, TYPE_ENUM)) != GRD5_READ_OK)
    return err;
  
  if ((*name = parse_typename(stream, &err)) == NULL)
    return err;
  
  if ((*subname = parse_typename(stream, &err)) == NULL)
    return err;

  return GRD5_READ_OK; 
}


static int parse_GrdL(FILE *stream, uint32_t *ngrad)
{
  int err;

  if ((err = parse_named_type(stream, "GrdL", TYPE_VAR_LEN_LIST)) != GRD5_READ_OK)
    return err;

  if ((err = parse_uint32(stream, ngrad)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Clrs(FILE *stream, uint32_t *nstop)
{
  int err;

  if ((err = parse_named_type(stream, "Clrs", TYPE_VAR_LEN_LIST)) != GRD5_READ_OK)
    return err;

  if ((err = parse_uint32(stream, nstop)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Trns(FILE *stream, uint32_t *ntrns)
{
  int err;

  if ((err = parse_named_type(stream, "Trns", TYPE_VAR_LEN_LIST)) != GRD5_READ_OK)
    return err;

  if ((err = parse_uint32(stream, ntrns)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Opct(FILE *stream, double *opacity)
{
  int err;

  if ((err = parse_untf(stream, "Opct", "#Prc", opacity)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Lctn(FILE *stream, uint32_t *location)
{
  int err;

  if ((err = parse_named_type(stream, "Lctn", TYPE_LONG)) != GRD5_READ_OK)
    return err;

  if ((err = parse_uint32(stream, location)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Mdpn(FILE *stream, uint32_t *midpoint)
{
  int err;

  if ((err = parse_named_type(stream, "Mdpn", TYPE_LONG)) != GRD5_READ_OK)
    return err;

  if ((err = parse_uint32(stream, midpoint)) != GRD5_READ_OK)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_Grad(FILE *stream)
{
  return parse_named_type(stream, "Grad", TYPE_OBJECT);
}

static int parse_Clr(FILE *stream)
{
  return parse_named_type(stream, "Clr ", TYPE_OBJECT);
}

static int parse_Nm(FILE *stream, grd5_string_t **ptitle)
{
  int err;

  if ((err = parse_named_type(stream, "Nm  ", TYPE_TEXT)) != GRD5_READ_OK)
    return err;

  grd5_string_t *title;

  if ((title = parse_ucs2(stream, &err)) == NULL)
    return err;

  *ptitle = title;

  return GRD5_READ_OK; 
}

static int parse_GrdF_GrdF_CstS(FILE *stream)
{
  int err;
  grd5_string_t *name, *subname = NULL;

  if ((err = parse_enum(stream, "GrdF", &name, &subname)) != GRD5_READ_OK)
    return err;

  bool matches = 
    typename_matches(name, "GrdF") &&
    typename_matches(subname, "CstS");

  grd5_string_destroy(name);
  grd5_string_destroy(subname);

  return (matches ? GRD5_READ_OK : GRD5_READ_PARSE);
}

static int parse_Type_Clry(FILE *stream, grd5_string_t **subname)
{
  int err;
  grd5_string_t *name;

  if ((err = parse_enum(stream, "Type", &name, subname)) != GRD5_READ_OK)
    return err;

  bool matches = typename_matches(name, "Clry");

  grd5_string_destroy(name);

  return (matches ? GRD5_READ_OK : GRD5_READ_PARSE);
}

static int parse_Intr(FILE *stream, double *interp)
{
  return parse_double(stream, "Intr", interp); 
}

typedef struct
{
  struct {
    grd5_string_t *display, *type;
  } name;
  uint32_t value;
} objc_t;

static void objc_destroy(objc_t *objc)
{
  grd5_string_destroy(objc->name.display);
  grd5_string_destroy(objc->name.type);
  free(objc);
}

static objc_t* parse_objc(FILE *stream, int *perr)
{
  grd5_string_t* dispname;

  if ((dispname = parse_ucs2(stream, perr)) == NULL)
    return NULL;

  grd5_string_t* typename;

  if ((typename = parse_typename(stream, perr)) == NULL)
    goto cleanup_dispname;

  uint32_t value;

  if ((*perr = parse_uint32(stream, &value)) != GRD5_READ_OK)
    goto cleanup_typename;

  objc_t* objc;

  if ((objc = malloc(sizeof(objc_t))) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      goto cleanup_typename;
    }

  objc->name.display = dispname;
  objc->name.type    = typename;
  objc->value        = value;

  return objc;

 cleanup_typename:
  grd5_string_destroy(typename);

 cleanup_dispname:
  grd5_string_destroy(dispname);

  return NULL;
}

static int parse_user_rgb(FILE *stream, grd5_rgb_t *rgb)
{
  int err = 
    (parse_double(stream, "Rd  ", &(rgb->Rd))  != GRD5_READ_OK) ||
    (parse_double(stream, "Grn ", &(rgb->Grn)) != GRD5_READ_OK) ||
    (parse_double(stream, "Bl  ", &(rgb->Bl))  != GRD5_READ_OK);
  
  printf("    %7.3f / %7.3f / %7.3f (%i)\n", 
	 rgb->Rd, rgb->Grn, rgb->Bl, err);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_hsb(FILE *stream, grd5_hsb_t *hsb)
{
  int err = 
    (parse_untf(stream, "H   ", "#Ang", &(hsb->H)) != GRD5_READ_OK) ||
    (parse_double(stream, "Strt", &(hsb->Strt)) != GRD5_READ_OK) ||
    (parse_double(stream, "Brgh", &(hsb->Brgh)) != GRD5_READ_OK);
  
  printf("    %7.3f / %7.3f / %7.3f (%i)\n", 
	 hsb->H, hsb->Strt, hsb->Brgh, err);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_colour(FILE *stream, grd5_colour_stop_t *stop)
{
  int err;
  objc_t *objc;

  if ((err = parse_Clr(stream)) != GRD5_READ_OK)
    return err;

  if ((objc = parse_objc(stream, &err)) == NULL)

  printf("    objc %*s %i\n", 
	 objc->name.type->len, 
	 objc->name.type->content, 
	 objc->value);

  uint32_t       ncomp = objc->value;
  grd5_string_t *model = objc->name.type;

  switch (ncomp)
    {
    case 3:

      if (grd5_string_matches(model, "RGBC"))
	{
	  stop->type = GRD5_STOP_RGB;
	  err = parse_user_rgb(stream, &(stop->u.rgb));
	}
      else if (grd5_string_matches(model, "HSBC"))
	{
	  stop->type = GRD5_STOP_HSB;
	  err = parse_user_hsb(stream, &(stop->u.hsb));
	}
      else 
	{
	  fprintf(stderr, "unknown user-colour type: %*s\n",
		  model->len, model->content);
	  err = GRD5_READ_PARSE;
	}

      break;

    default:
      err = GRD5_READ_PARSE;
    }
  
  objc_destroy(objc);
  
  return err;
}

static int parse_colour_stop(FILE *stream, grd5_colour_stop_t *stop)
{
  int err, type;
  objc_t *objc;

  if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
    return err;
  
  if (type != TYPE_OBJECT) return GRD5_READ_PARSE;
  
  if ((objc = parse_objc(stream, &err)) == NULL)
    return err;
  
  uint32_t ncomp = objc->value;
  
  printf("    objc %*s %i\n", 
	 objc->name.type->len, 
	 objc->name.type->content, 
	 objc->value);
  
  objc_destroy(objc);

  bool have_user_colour;

  switch (ncomp)
    {
    case 4:
      have_user_colour = true;
      break;
    case 3:
      have_user_colour = false;
      break;
    default:
      return GRD5_READ_PARSE;
    }

  if (have_user_colour &&
      (err = parse_user_colour(stream, stop)) != GRD5_READ_OK)
    return err;

  grd5_string_t* subtype;

  if ((err = parse_Type_Clry(stream, &subtype)) != GRD5_READ_OK)
    return err;

  if (have_user_colour && ! typename_matches(subtype, "UsrS"))
    {
      fprintf(stderr, "read a user colour, but type is %*s not UsrS\n",
	      subtype->len, subtype->content);
      return GRD5_READ_PARSE;
    }

  printf("    Clry %*s\n", subtype->len, subtype->content);

  if ((err = parse_Lctn(stream, &(stop->location))) != GRD5_READ_OK)
    return err;

  printf("    Lctn %i\n", stop->location);

  if ((err = parse_Mdpn(stream, &(stop->midpoint))) != GRD5_READ_OK)
    return err;

  printf("    Mdpn %i\n", stop->midpoint);

  return GRD5_READ_OK;
}

static int parse_transp_stop(FILE *stream, grd5_transp_stop_t *stop)
{
  int err, type;
  objc_t *objc;

  if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
    return err;
  
  if (type != TYPE_OBJECT) return GRD5_READ_PARSE;
  
  if ((objc = parse_objc(stream, &err)) == NULL)
    return err;
  
  uint32_t ncomp = objc->value;

  printf("    objc %*s %i\n", 
	 objc->name.type->len, 
	 objc->name.type->content, 
	 objc->value);

  if (ncomp != 3)
    {
      fprintf(stderr, "can't handle %i component transparency stop\n", ncomp);
      return GRD5_READ_PARSE;
    }

  if (! typename_matches(objc->name.type, "TrnS"))
    return GRD5_READ_PARSE;

  objc_destroy(objc);

  if ((err = parse_Opct(stream, &(stop->Opct))) != GRD5_READ_OK)
    return err;

  printf("    Opct %.3f\n", stop->Opct);

  if ((err = parse_Lctn(stream, &(stop->Lctn))) != GRD5_READ_OK)
    return err;

  printf("    Lctn %i\n", stop->Lctn);

  if ((err = parse_Mdpn(stream, &(stop->Mdpt))) != GRD5_READ_OK)
    return err;

  printf("    Mdpn %i\n", stop->Mdpt);

  return GRD5_READ_OK;
}

static int grd5_stream(FILE* stream, grd5_t* grd5)
{
  int err;
  char cbuf[4];
 
  /* magic number */

  if (fread(cbuf, 1, 4, stream) != 4)
    return GRD5_READ_FREAD;

  if (strncmp(cbuf, "8BGR", 4) != 0)
    return GRD5_READ_NOT_GRD;

  /* version */

  uint16_t version;

  if ((err = parse_uint16(stream, &version)) != GRD5_READ_OK)
    return err;

  if (version != 5)
    return GRD5_READ_NOT_GRD5;

  /* skip unknown stuff */

  if (fseek(stream, 22L, SEEK_CUR) != 0)
    return GRD5_READ_FREAD;

  /* gradient list header */

  if ((err = parse_GrdL(stream, &(grd5->n)))  != GRD5_READ_OK)
    return err;
  
  if ((grd5->gradients = malloc((grd5->n) * sizeof(grd5_grad_t))) == NULL)
    return GRD5_READ_MALLOC;

  printf("%i gradients\n", grd5->n);

  int i;
  
  for (i = 0 ; i < grd5->n ; i++)
    {
      int type;

      grd5_grad_t *grad = grd5->gradients + i;

      /* outer objc */

      if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
	return err;

      if (type != TYPE_OBJECT) return GRD5_READ_PARSE;

      objc_t *objc;

      if ((objc = parse_objc(stream, &err)) == NULL)
	return err;

      printf("  objc %*s %i\n", 
	     objc->name.type->len, 
	     objc->name.type->content, 
	     objc->value);

      objc_destroy(objc);

      /* gradient container */

      if ((err = parse_Grad(stream)) != GRD5_READ_OK)
	return err;

      /* inner objc */

      if ((objc = parse_objc(stream, &err)) == NULL)
	return err;

      uint32_t ncomp = objc->value;

      printf("  objc %*s %i\n", 
	     objc->name.type->len, 
	     objc->name.type->content, 
	     objc->value);

      objc_destroy(objc);

      switch (ncomp)
	{
	  int j;

	case 5:

	  /* gradient title */

	  if ((err = parse_Nm(stream, &(grad->title))) != GRD5_READ_OK)
	    return err;

	  /* gradient form */

	  if ((err = parse_GrdF_GrdF_CstS(stream)) != GRD5_READ_OK)
	    return err;

	  /* gradient interpolation */

	  if ((err = parse_Intr(stream, &(grad->interp))) != GRD5_READ_OK)
	    return err;

	  printf("  Inrp %.3f\n", grad->interp);

	  /* number of stops */

	  if ((err = parse_Clrs(stream, &(grad->colour.n))) != GRD5_READ_OK)
	    return err;

	  grad->colour.stops = malloc(grad->colour.n * sizeof(grd5_colour_stop_t));

	  if (grad->colour.stops == NULL)
	    return GRD5_READ_MALLOC;

	  printf("  Clrs %d\n", grad->colour.n);

	  for (j=0 ; j < grad->colour.n ; j++)
	    {
	      if ((err = parse_colour_stop(stream, grad->colour.stops+j)) != GRD5_READ_OK)
		return err;
	    }

	  /* number of transparency samples */

	  if ((err = parse_Trns(stream, &(grad->transp.n))) != GRD5_READ_OK)
	    return err;

	  grad->transp.stops = malloc(grad->transp.n * sizeof(grd5_transp_stop_t));

	  if (grad->transp.stops == NULL)
	    return GRD5_READ_MALLOC;

	  printf("  Trns %d\n", grad->transp.n);

	  for (j=0 ; j < grad->transp.n ; j++)
	    {
	      if ((err = parse_transp_stop(stream, grad->transp.stops+j)) != GRD5_READ_OK)
		return err;	
	    }

	  break;
	case 3:
	default:
	  return GRD5_READ_PARSE; 
	}
    }

  return GRD5_READ_OK;
}

