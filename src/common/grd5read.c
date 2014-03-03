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

// #define DEBUG

#ifdef DEBUG
#define debug_error(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug_error(...)
#endif

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
    {
      debug_error("failed read\n");
      grd5_destroy(grd5);
    }

  return err;
}

/*
  read_<type> functions read a particular data type (double,
  uint32, etc) from the stream
*/

static int read_uint16(FILE *stream, uint16_t *i)
{
  uint16_t buf;

  if (fread(&buf, 2, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *i = be16toh(buf);

  return GRD5_READ_OK;
}

static int read_uint32(FILE *stream, uint32_t *i)
{
  uint32_t buf;

  if (fread(&buf, 4, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *i = be32toh(buf);

  return GRD5_READ_OK;
}

static int read_type(FILE *stream, int *type)
{
  char buf[4];

  if (fread(&buf, 1, 4, stream) != 4)
    return GRD5_READ_FREAD;

  *type = grd5_type(buf);

  return GRD5_READ_OK;
}


static int read_double(FILE *stream, double *pval)
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

static int read_bool(FILE *stream, bool *pval)
{
  unsigned char cval;

  if (fread(&cval, 1, 1, stream) != 1) return GRD5_READ_FREAD;

  *pval = ! (cval == 0);

  return GRD5_READ_OK;
}

/*
  a number of functions reading grd5_string_t of various formats
*/

typedef enum { 
  grd5_string_typename, 
  grd5_string_ucs2,
  grd5_string_tdta
} grd5_string_type_t;

static grd5_string_t* parse_grd5_string(FILE *stream, 
					grd5_string_type_t type,
					int *perr)
{
  uint32_t len;

  if ((*perr = read_uint32(stream, &len)) != GRD5_READ_OK)
    return NULL;

  switch (type)
    {
    case grd5_string_tdta :
      break;
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

static grd5_string_t* parse_tdta(FILE *stream, int *perr)
{
  return parse_grd5_string(stream, grd5_string_tdta, perr);
}

/* helper functions for parse_<type> functions */

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
	{
	  debug_error("expected type '%s', read '%*s'\n",
		      expected_name, typename->len, typename->content);
	  grd5_string_destroy(typename);
	  return GRD5_READ_PARSE;
	}
    }

  int type;

  if ((err = read_type(stream, &type)) != GRD5_READ_OK)
    return err;

  if (type == TYPE_UNKNOWN)
    debug_error("unknown type\n");

  if (type != expected_type)
    return GRD5_READ_PARSE;

  return GRD5_READ_OK;
}

/*
  parsing particular tokens, typically of the form [size] [name] [value]
  or similar.  Depending on the application we may pass [name] in as
  an expected value, somteimes we return it (by reference) instead
*/

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
      debug_error("expected unit %s, read %4s\n", expected_unit, unit);
      return GRD5_READ_PARSE;
    }

  return read_double(stream, pval);
}

static int parse_double(FILE *stream, const char* expected_name, double *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_DOUBLE)) != GRD5_READ_OK)
    return err;

  return read_double(stream, pval);
}

static int parse_bool(FILE *stream, const char* expected_name, bool* pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_BOOL)) != GRD5_READ_OK)
    return err;

  return read_bool(stream, pval);
}

static int parse_long(FILE *stream, 
		      const char *expected_name, 
		      uint32_t *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_LONG)) != GRD5_READ_OK)
    return err;

  return read_uint32(stream, pval);
}

static int parse_vll_length(FILE *stream, 
			    const char *expected_name, 
			    uint32_t *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_VAR_LEN_LIST)) != GRD5_READ_OK)
    return err;

  return read_uint32(stream, pval);
}

static int parse_enum(FILE *stream, 
		      const char* expected_typename,
		      grd5_string_t** pname,
		      grd5_string_t** psubname)
{
  int err;
  grd5_string_t *name, *subname;

  if ((err = parse_named_type(stream, expected_typename, TYPE_ENUM)) != GRD5_READ_OK)
    return err;
  
  if ((name = parse_typename(stream, &err)) != NULL)
    {
      if ((subname = parse_typename(stream, &err)) != NULL)
	{
	  *pname    = name;
	  *psubname = subname;
	  return GRD5_READ_OK; 
	}
      else
	grd5_string_destroy(name);
    }

  return err;
}

static int parse_text(FILE *stream,
		      const char *expected_name,
		      grd5_string_t **pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_TEXT)) != GRD5_READ_OK)
    return err;

  grd5_string_t *val;

  if ((val = parse_ucs2(stream, &err)) == NULL)
    return err;

  *pval = val;

  return GRD5_READ_OK; 
}

static int parse_extremum(FILE *stream, const char* name, grd5_extremum_t *ext)
{
  int err;

  if ((err = parse_named_type(stream, name, TYPE_VAR_LEN_LIST)) != GRD5_READ_OK)
    return err;

  uint32_t n;

  if ((err = read_uint32(stream, &n)) != GRD5_READ_OK)
    return err;
  
  uint32_t i, vals[n]; 

  for (i=0 ; i<n ; i++)
    {
      int type;

      if ((err = read_type(stream, &type)) != GRD5_READ_OK)
	return err;

      if (type != TYPE_LONG)
	return GRD5_READ_PARSE;

      if ((err = read_uint32(stream, vals+i)) !=  GRD5_READ_OK)
	return err;
    }

  size_t sz = n * sizeof(uint32_t);

  if ((ext->vals = malloc(sz)) == NULL)
    return GRD5_READ_MALLOC;

  memcpy(ext->vals, vals, sz);

  ext->n = n;

  return GRD5_READ_OK;
}

/*
  particular token parsers, constructed from the above
*/

static int parse_ShTr(FILE *stream, bool *pval)
{
  return parse_bool(stream, "ShTr", pval);
}

static int parse_VctC(FILE *stream, bool *pval)
{
  return parse_bool(stream, "VctC", pval);
}

static int parse_GrdL(FILE *stream, uint32_t *ngrad)
{
  return parse_vll_length(stream, "GrdL", ngrad);
}

static int parse_Clrs(FILE *stream, uint32_t *nstop)
{
  return parse_vll_length(stream, "Clrs", nstop);
}

static int parse_ClrS(FILE *stream, grd5_string_t **pmodel)
{
  int err;
  grd5_string_t *name, *subname = NULL;

  if ((err = parse_enum(stream, "ClrS", &name, &subname)) != GRD5_READ_OK)
    return err;

  bool matches = typename_matches(name, "ClrS");

  grd5_string_destroy(name);

  if (matches)
    {
      *pmodel = subname;
      return GRD5_READ_OK;
    }
  else
    {
      grd5_string_destroy(subname);
      return GRD5_READ_PARSE;
    }
}

static int parse_Trns(FILE *stream, uint32_t *ntrns)
{
  return parse_vll_length(stream, "Trns", ntrns);
}

static int parse_Opct(FILE *stream, double *opacity)
{
  return parse_untf(stream, "Opct", "#Prc", opacity);
}

static int parse_Lctn(FILE *stream, uint32_t *location)
{
  return parse_long(stream, "Lctn", location);
}

static int parse_Mdpn(FILE *stream, uint32_t *midpoint)
{
  return parse_long(stream, "Mdpn", midpoint);
}

static int parse_RndS(FILE *stream, uint32_t *rnds)
{
  return parse_long(stream, "RndS", rnds);
}

static int parse_Smth(FILE *stream, uint32_t *smth)
{
  return parse_long(stream, "Smth", smth);
}

static int parse_Mnm(FILE *stream, grd5_extremum_t *min)
{
  return parse_extremum(stream, "Mnm ", min);
}

static int parse_Mxm(FILE *stream, grd5_extremum_t *max)
{
  return parse_extremum(stream, "Mxm ", max);
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
  return parse_text(stream, "Nm  ", ptitle);
}

static int parse_GrdF_GrdF(FILE *stream, grd5_string_t **pgtype)
{
  int err;
  grd5_string_t *name, *subname = NULL;

  if ((err = parse_enum(stream, "GrdF", &name, &subname)) != GRD5_READ_OK)
    return err;

  bool matches = typename_matches(name, "GrdF");

  grd5_string_destroy(name);

  if (matches)
    {
      *pgtype = subname;
      return GRD5_READ_OK;
    }
  else
    {
      grd5_string_destroy(subname);
      return GRD5_READ_PARSE;
    }
}

static int parse_Type_Clry(FILE *stream, grd5_string_t **pctype)
{
  int err;
  grd5_string_t *name, *subname = NULL;

  if ((err = parse_enum(stream, "Type", &name, &subname)) != GRD5_READ_OK)
    return err;

  bool matches = typename_matches(name, "Clry");

  grd5_string_destroy(name);

  if (matches)
    {
      *pctype = subname;
      return GRD5_READ_OK;
    }
  else
    {
      grd5_string_destroy(subname);
      return GRD5_READ_OK;
    }
}

static int parse_Intr(FILE *stream, double *interp)
{
  return parse_double(stream, "Intr", interp); 
}

/*
  Objc is a bit of a special case since it is rather complicated
*/

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

  if ((*perr = read_uint32(stream, &value)) != GRD5_READ_OK)
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

/*
  user colours
*/

static int parse_user_rgb(FILE *stream, grd5_rgb_t *rgb)
{
  int err = 
    (parse_double(stream, "Rd  ", &(rgb->Rd))  != GRD5_READ_OK) ||
    (parse_double(stream, "Grn ", &(rgb->Grn)) != GRD5_READ_OK) ||
    (parse_double(stream, "Bl  ", &(rgb->Bl))  != GRD5_READ_OK);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_hsb(FILE *stream, grd5_hsb_t *hsb)
{
  int err = 
    (parse_untf(stream, "H   ", "#Ang", &(hsb->H)) != GRD5_READ_OK) ||
    (parse_double(stream, "Strt", &(hsb->Strt)) != GRD5_READ_OK) ||
    (parse_double(stream, "Brgh", &(hsb->Brgh)) != GRD5_READ_OK);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_lab(FILE *stream, grd5_lab_t *lab)
{
  int err = 
    (parse_double(stream, "Lmnc", &(lab->Lmnc)) != GRD5_READ_OK) ||
    (parse_double(stream, "A   ", &(lab->A   )) != GRD5_READ_OK) ||
    (parse_double(stream, "B   ", &(lab->B   )) != GRD5_READ_OK);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_cmyc(FILE *stream, grd5_cmyc_t *cmyc)
{
  int err = 
    (parse_double(stream, "Cyn ", &(cmyc->Cyn))  != GRD5_READ_OK) ||
    (parse_double(stream, "Mgnt", &(cmyc->Mgnt)) != GRD5_READ_OK) ||
    (parse_double(stream, "Ylw ", &(cmyc->Ylw))  != GRD5_READ_OK) ||
    (parse_double(stream, "Blck", &(cmyc->Blck)) != GRD5_READ_OK);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_grsc(FILE *stream, grd5_grsc_t *grsc)
{
  int err = (parse_double(stream, "Gry ", &(grsc->Gry)) != GRD5_READ_OK);
  
  return (err ?  GRD5_READ_PARSE :  GRD5_READ_OK);
}

static int parse_user_book(FILE *stream, grd5_book_t *book)
{
  int err;

  if ((err = parse_text(stream, "Bk  ", &(book->Bk))) != GRD5_READ_OK)
    return err;

  if ((err = parse_text(stream, "Nm  ", &(book->Nm))) != GRD5_READ_OK)
    return err;

  if ((err = parse_long(stream, "bookID", &(book->bookID))) != GRD5_READ_OK)
    return err;

  if ((err = parse_named_type(stream, "bookKey", TYPE_TDTA)) != GRD5_READ_OK)
    return err;
     
  if ((book->bookKey = parse_tdta(stream, &err)) == NULL)
    return err;
  
  return GRD5_READ_OK;
}

static int parse_user_colour(FILE *stream, grd5_colour_stop_t *stop)
{
  int err;
  objc_t *objc;

  if ((err = parse_Clr(stream)) != GRD5_READ_OK)
    return err;

  if ((objc = parse_objc(stream, &err)) == NULL)
    return err;

  uint32_t ncomp = objc->value;
  grd5_string_t *model_name = objc->name.type;
  int model = grd5_model(model_name);

  if (model == GRD5_MODEL_UNKNOWN)
    {
      debug_error("bad colour model %*s\n", 
		  model_name->len,
		  model_name->content);	      
      objc_destroy(objc);
      return GRD5_READ_PARSE;
    }

  switch (ncomp)
    {
    case 4:

      switch(model)
	{
	case GRD5_MODEL_CMYC:
	  stop->type = GRD5_MODEL_CMYC;
	  err = parse_user_cmyc(stream, &(stop->u.cmyc));
	  break;
	case GRD5_MODEL_BOOK:
	  stop->type = GRD5_MODEL_BOOK;
	  err = parse_user_book(stream, &(stop->u.book));
	  break;
	default:
	  err = GRD5_READ_PARSE;
	}
      break;

    case 3:

      switch(model)
	{
	case GRD5_MODEL_RGB:
	  stop->type = GRD5_MODEL_RGB;
	  err = parse_user_rgb(stream, &(stop->u.rgb));
	  break;
	case  GRD5_MODEL_HSB:
	  stop->type = GRD5_MODEL_HSB;
	  err = parse_user_hsb(stream, &(stop->u.hsb));
	  break;
	case GRD5_MODEL_LAB:
	  stop->type = GRD5_MODEL_LAB;
	  err = parse_user_lab(stream, &(stop->u.lab));
	  break;
	default:
	  err = GRD5_READ_PARSE;
	}
      break;

    case 1:

      switch(model)
	{
	case GRD5_MODEL_GRSC:
	  stop->type = GRD5_MODEL_GRSC;
	  err = parse_user_grsc(stream, &(stop->u.grsc));
	  break;
	default:
	  err = GRD5_READ_PARSE;
	}
      break;
      
    default:
      err = GRD5_READ_PARSE;
    }
  
  objc_destroy(objc);
  
  return err;
}

/*
  stops
*/

static int parse_colour_stop(FILE *stream, grd5_colour_stop_t *stop)
{
  int err, type;
  objc_t *objc;

  if ((err = read_type(stream, &type)) != GRD5_READ_OK)
    return err;
  
  if (type != TYPE_OBJECT) return GRD5_READ_PARSE;
  
  if ((objc = parse_objc(stream, &err)) == NULL)
    return err;
  
  uint32_t ncomp = objc->value;

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
      debug_error("unexpected number of stop componets (%i)", ncomp);
      return GRD5_READ_PARSE;
    }

  if (have_user_colour &&
      (err = parse_user_colour(stream, stop)) != GRD5_READ_OK)
    return err;

  grd5_string_t* subtype = NULL;

  if ((err = parse_Type_Clry(stream, &subtype)) != GRD5_READ_OK)
    return err;

  if (have_user_colour)
    {
      if (! typename_matches(subtype, "UsrS"))
	{
	  debug_error("read a user colour, but type is %*s not UsrS\n",
		      subtype->len, subtype->content);
	  return GRD5_READ_PARSE;
	}
    }
  else
    {
      if (typename_matches(subtype, "BckC"))
	{
	  stop->type = GRD5_MODEL_BCKC;
	}
      else if (typename_matches(subtype, "FrgC"))
	{
	  stop->type = GRD5_MODEL_FRGC;
	}
      else
	{
	  debug_error("read a non-user colour, but type is %*s not BckC/FrgC\n",
		      subtype->len, subtype->content);
	  return GRD5_READ_PARSE;
	}
    }

  grd5_string_destroy(subtype);

  if ((err = parse_Lctn(stream, &(stop->Lctn))) != GRD5_READ_OK)
    return err;

  if ((err = parse_Mdpn(stream, &(stop->Mdpn))) != GRD5_READ_OK)
    return err;

  return GRD5_READ_OK;
}

static int parse_transp_stop(FILE *stream, grd5_transp_stop_t *stop)
{
  int err, type;
  objc_t *objc;

  if ((err = read_type(stream, &type)) != GRD5_READ_OK)
    return err;
  
  if (type != TYPE_OBJECT) return GRD5_READ_PARSE;
  
  if ((objc = parse_objc(stream, &err)) == NULL)
    return err;
  
  uint32_t ncomp = objc->value;

  if (ncomp != 3)
    {
      debug_error("can't handle %i component transparency stop\n", ncomp);
      return GRD5_READ_PARSE;
    }

  if (! typename_matches(objc->name.type, "TrnS"))
    return GRD5_READ_PARSE;

  objc_destroy(objc);

  if ((err = parse_Opct(stream, &(stop->Opct))) != GRD5_READ_OK)
    return err;

  if ((err = parse_Lctn(stream, &(stop->Lctn))) != GRD5_READ_OK)
    return err;

  if ((err = parse_Mdpn(stream, &(stop->Mdpn))) != GRD5_READ_OK)
    return err;

  return GRD5_READ_OK;
}

/*
  parse a file
*/

static int grd5_stream(FILE* stream, grd5_t* grd5)
{
  int err;
  char cbuf[4];
 
  /* magic number */

  if (fread(cbuf, 1, 4, stream) != 4)
    {
      debug_error("fread of magic number\n");
      return GRD5_READ_FREAD;
    }

  if (strncmp(cbuf, "8BGR", 4) != 0)
    {
      debug_error("magic number %4s\n", cbuf);
      return GRD5_READ_NOT_GRD;
    }

  /* version */

  uint16_t version;

  if ((err = read_uint16(stream, &version)) != GRD5_READ_OK)
    {
      debug_error("fread of version\n");
      return err;
    }

  if (version != 5)
    {
      debug_error("magic number %i\n", version);
      return GRD5_READ_NOT_GRD5;
    }

  /* skip unknown stuff */

  if (fseek(stream, 22L, SEEK_CUR) != 0)
    {
      debug_error("header seek\n");
      return GRD5_READ_FREAD;
    }

  /* gradient list header */

  if ((err = parse_GrdL(stream, &(grd5->n)))  != GRD5_READ_OK)
    {
      debug_error("GrdL\n");
      return err;
    }
  
  if ((grd5->gradients = malloc((grd5->n) * sizeof(grd5_grad_t))) == NULL)
    {
      debug_error("malloc\n");
      return GRD5_READ_MALLOC;
    }

  int i;
  
  for (i = 0 ; i < grd5->n ; i++)
    {
      int type;
      grd5_grad_t *grad = grd5->gradients + i;

      /* outer objc */

      if ((err = read_type(stream, &type)) != GRD5_READ_OK)
	{
	  debug_error("reading outer Objc\n");
	  return err;
	}

      if (type != TYPE_OBJECT)
	{
	  debug_error("outer Objc not found\n");
	  return GRD5_READ_PARSE;
	}

      objc_t *objc;

      if ((objc = parse_objc(stream, &err)) == NULL)
	{
	  debug_error("Objc\n");
	  return err;
	}

      objc_destroy(objc);

      /* gradient container */

      if ((err = parse_Grad(stream)) != GRD5_READ_OK)
	{
	  debug_error("Grad\n");
	  return err;
	}

      /* inner objc */

      if ((objc = parse_objc(stream, &err)) == NULL)
	{
	  debug_error("inner Objc\n");
	  return err;
	}

      uint32_t ncomp = objc->value;

      objc_destroy(objc);

      /* gradient title */

      if ((err = parse_Nm(stream, &grad->title)) != GRD5_READ_OK)
	{
	  debug_error("Nm\n");
	  return err;
	}

      /* gradient form */
      
      grd5_string_t *gradient_type = NULL;

      if ((err = parse_GrdF_GrdF(stream, &gradient_type)) != GRD5_READ_OK)
	{
	  debug_error("GrdF\n");
	  return err;
	}

      if (typename_matches(gradient_type, "CstS"))
	{
	  if (ncomp != 5)
	    {
	      debug_error("CstS with %i component\n", ncomp);
	      return GRD5_READ_PARSE;
	    }

	  grad->type = GRD5_GRAD_CUSTOM;
	  grd5_grad_custom_t *gradc = &(grad->u.custom);

	  /* gradient interpolation */

	  if ((err = parse_Intr(stream, &(gradc->interp))) != GRD5_READ_OK)
	    {
	      debug_error("Intr\n");
	      return err;
	    }

	  /* number of stops */

	  uint32_t nstop;

	  if ((err = parse_Clrs(stream, &nstop)) != GRD5_READ_OK)
	    {
	      debug_error("Clrs\n");
	      return err;
	    }

	  gradc->colour.stops = NULL;

	  if (nstop > 0)
	    {
	      int j;
	      grd5_colour_stop_t stops[nstop];

	      for (j=0 ; j < nstop ; j++)
		{
		  if ((err = parse_colour_stop(stream, stops+j)) != GRD5_READ_OK)
		    {
		      debug_error("parse of stop %i\n", j);
		      return err;
		    }
		}

	      size_t stops_size = nstop * sizeof(grd5_colour_stop_t);

	      if ((gradc->colour.stops = malloc(stops_size)) == NULL) 
		{
		  debug_error("malloc\n");
		  return GRD5_READ_MALLOC;
		}

	      if (memcpy(gradc->colour.stops, stops, stops_size) == NULL)
		{
		  debug_error("memcpy\n");
		  return GRD5_READ_MALLOC;
		}

	      gradc->colour.n = nstop;
	    }

	  /* number of transparency samples */

	  if ((err = parse_Trns(stream, &(gradc->transp.n))) != GRD5_READ_OK)
	    {
	      debug_error("Trns\n");
	      return err;
	    }

	  gradc->transp.stops = NULL;

	  if (gradc->transp.n > 0)
	    {
	      int j;
	      grd5_transp_stop_t stops[gradc->transp.n];

	      for (j=0 ; j < gradc->transp.n ; j++)
		{
		  if ((err = parse_transp_stop(stream, stops+j)) != GRD5_READ_OK)
		    {
		      debug_error("transparency stop\n");
		      return err;
		    }
		}

	      size_t stops_size = gradc->transp.n * sizeof(grd5_transp_stop_t);

	      if ((gradc->transp.stops = malloc(stops_size)) == NULL)
		{
		  debug_error("malloc\n");
		  return GRD5_READ_MALLOC;
		}

	      if (memcpy(gradc->transp.stops, stops, stops_size) == NULL)
		{
		  debug_error("memcpy\n");
		  return GRD5_READ_MALLOC;
		}
	    }
	}
      else if (typename_matches(gradient_type, "ClNs"))
	{
	  if (ncomp != 9)
	    {
	      debug_error("ClNs with %i component\n", ncomp);
	      return GRD5_READ_PARSE;
	    }

	  grad->type = GRD5_GRAD_NOISE;
	  grd5_grad_noise_t *gradn = &(grad->u.noise);

	  gradn->min.n = gradn->max.n = 0;

	  err = parse_ShTr(stream, &(gradn->show_transparency));
	  if (err != GRD5_READ_OK)
	    {
	      debug_error("ShTr\n");
	      return err;
	    }

	  if ((err = parse_VctC(stream, &(gradn->vector_colour))) != GRD5_READ_OK)
	    {
	      debug_error("VctC\n");
	      return err;
	    }

	  grd5_string_t *model_name = NULL;

	  if ((err = parse_ClrS(stream, &model_name)) != GRD5_READ_OK)
	    {
	      debug_error("Clrs\n");
	      return err;
	    }

	  if ((gradn->model = grd5_model(model_name)) == GRD5_MODEL_UNKNOWN)
	    {
	      debug_error("model name %*s\n", 
			  model_name->len, model_name->content);
	    }

	  grd5_string_destroy(model_name);

	  if (gradn->model == GRD5_MODEL_UNKNOWN)
	    {
	      debug_error("unknown model\n");
	      return GRD5_READ_PARSE;
	    }

	  if ((err = parse_RndS(stream, &(gradn->seed))) != GRD5_READ_OK)
	    {
	      debug_error("RndS\n");
	      return err;
	    }

	  if ((err = parse_Smth(stream, &(gradn->smoothness))) != GRD5_READ_OK)
	    {
	      debug_error("Smth\n");
	      return err;
	    }

	  if ((err = parse_Mnm(stream, &(gradn->min)))  != GRD5_READ_OK)
	    {
	      debug_error("Mnm\n");
	      return err;
	    }

	  if ((err = parse_Mxm(stream, &(gradn->max)))  != GRD5_READ_OK)
	    {
	      debug_error("Mxm\n");
	      return err;
	    }
	}
      else
	{
	  debug_error("unknown gradient format %*s\n", 
		      gradient_type->len,
		      gradient_type->content);
	  return GRD5_READ_PARSE; 
	}

      grd5_string_destroy(gradient_type);

    }

  return GRD5_READ_OK;
}
