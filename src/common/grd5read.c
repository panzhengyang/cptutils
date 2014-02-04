/*
  grd5read.c
  Copyright (c) J.J. Green 2014
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "grd5read.h"
#include "grd5type.h"

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

extern int grd5_read(const char* file, grd5_t* grd5)
{
  int err = GRD5_READ_BUG;

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

typedef struct
{
  uint32_t len;
  char *content;
} grd5_string_t;

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

static void grd5_string_destroy(grd5_string_t* gstr)
{
  free(gstr->content);
  free(gstr);
}

static bool typename_matches(grd5_string_t* typename, const char* expected)
{
  if (strncmp(typename->content, expected, typename->len) != 0)
    {
      fprintf(stderr, "read typename %*s, expecting %s\n",
	      typename->len, typename->content, expected);
      return false;
    }
  return true;
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
      grd5_string_destroy(typename);
      if (!matches) return GRD5_READ_PARSE;
    }

  int type;

  if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
    return err;

  if (type != expected_type)
    return GRD5_READ_PARSE;

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

static int parse_Grad(FILE *stream)
{
  return parse_named_type(stream, "Grad", TYPE_OBJECT);
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

static int parse_enum(FILE *stream, 
		      const char* expected_name, 
		      const char* expected_subname)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_ENUM)) != GRD5_READ_OK)
    return err;

  grd5_string_t *typename;

  if ((typename = parse_typename(stream, &err)) == NULL)
    return err;
  else
    {
      bool matches = typename_matches(typename, expected_name);
      grd5_string_destroy(typename);
      if (!matches) return GRD5_READ_PARSE;
    }

  if ((typename = parse_typename(stream, &err)) == NULL)
    return err;
  else
    {
      bool matches = typename_matches(typename, expected_subname);
      grd5_string_destroy(typename);
      if (!matches) return GRD5_READ_PARSE;
    }

  return GRD5_READ_OK; 
}

static int parse_GrdF_CstS(FILE *stream)
{
  return parse_enum(stream, "GrdF", "CstS"); 
}


static int parse_double(FILE *stream, const char* expected_name, double *pval)
{
  int err;

  if ((err = parse_named_type(stream, expected_name, TYPE_DOUBLE)) != GRD5_READ_OK)
    return err;

  double val;

  if (fread(&val, 8, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *pval = (double)be64toh((uint64_t)val);

  return GRD5_READ_OK;
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

  uint32_t ngrad;

  if ((err = parse_GrdL(stream, &ngrad))  != GRD5_READ_OK)
    return err;

  printf("%i gradients\n", ngrad);

  int i;
  
  for (i=0 ; i<ngrad ; i++)
    {
      int type;

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
	  grd5_string_t *title;
	  double interp;

	case 5:

	  /* gradient title */

	  if ((err = parse_Nm(stream, &title)) != GRD5_READ_OK)
	    return err;

	  /* gradient form */

	  if ((err = parse_GrdF_CstS(stream)) != GRD5_READ_OK)
	    return err;

	  /* gradient inerpolation */

	  if ((err = parse_Intr(stream, &interp)) != GRD5_READ_OK)
	    return err;

	  printf("  Inrp %f\n", interp);

	  break;
	case 3:
	default:
	  return GRD5_READ_PARSE; 
	}
    }

  return GRD5_READ_OK;
}

