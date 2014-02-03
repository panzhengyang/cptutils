/*
  grd5read.c
  Copyright (c) J.J. Green 2014
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "grd5read.h"
#include "grd5type.h"
#include "htons.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <uchar.h>

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

  *i = ntohs(buf);

  return GRD5_READ_OK;
}

static int parse_uint32(FILE *stream, uint32_t *i)
{
  uint32_t buf;

  if (fread(&buf, 4, 1, stream) != 1)
    return GRD5_READ_FREAD;

  *i = ntohl(buf);

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

typedef struct
{
  int type;
  uint32_t namelen;
  char *name;
} token_t;

static void token_destroy(token_t *token)
{
  free(token->name);
  free(token);
}

static bool token_matches(token_t *token, 
			  const char* expected_name,
			  int expected_type)
{
  if (token->type != expected_type)
    {
      fprintf(stderr, "token not of correct type\n");
      return false;
    }

  if (strncmp(token->name, expected_name, token->namelen) != 0)
    {
      fprintf(stderr, "read token %*s, expecting %s\n",
	      token->namelen, token->name, expected_name);
      return false;
    }

  return true;
}


static token_t* parse_token(FILE *stream, int *perr)
{
  uint32_t namelen;

  if ((*perr = parse_uint32(stream, &namelen)) != GRD5_READ_OK)
    return NULL;

  if (namelen == 0) namelen = 4;

  char *name;

  if ((name = malloc(namelen)) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      return NULL;
    }

  if (fread(name, 1, namelen, stream) != namelen)
    {
      *perr = GRD5_READ_FREAD;
      goto cleanup_name;
    }

  token_t* token;

  if ((token = malloc(sizeof(token_t))) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      goto cleanup_name;
    }

  int type;

  if ((*perr = parse_type(stream, &type)) != GRD5_READ_OK)
    goto cleanup_token;

  token->namelen = namelen;
  token->name    = name;
  token->type    = type;

  return token;

 cleanup_token:

  free(token);

 cleanup_name:

  free(name);

  return NULL;
}

typedef struct
{
  uint32_t namelen;
  char16_t *name;
} display_t;

typedef struct
{
  display_t display;
  uint32_t namelen;
  char *name;
  uint32_t value;
} objc_t;

static void objc_destroy(objc_t *objc)
{
  free(objc->display.name);
  free(objc->name);
  free(objc);
}

static objc_t* parse_objc(FILE *stream, int *perr)
{
  display_t display;

  if ((*perr = parse_uint32(stream, &(display.namelen))) != GRD5_READ_OK)
    return NULL;

  if ((display.name = malloc(2*display.namelen)) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      return NULL;
    }

  if (fread(display.name, 2, display.namelen, stream) != display.namelen)
    {
      *perr = GRD5_READ_FREAD;
      goto cleanup_display;
    }

  uint32_t namelen;

  if ((*perr = parse_uint32(stream, &namelen)) != GRD5_READ_OK)
    goto cleanup_display;

  if (namelen == 0) namelen = 4;

  char *name;

  if ((name = malloc(namelen)) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      goto cleanup_display;
    }

  if (fread(name, 1, namelen, stream) != namelen)
    {
      *perr = GRD5_READ_FREAD;
      goto cleanup_name;
    }

  uint32_t value;

  if ((*perr = parse_uint32(stream, &value)) != GRD5_READ_OK)
    goto cleanup_name;

  objc_t* objc;

  if ((objc = malloc(sizeof(objc_t))) == NULL)
    {
      *perr = GRD5_READ_MALLOC;
      goto cleanup_name;
    }

  objc->display = display;
  objc->name    = name;
  objc->value   = value;

  return objc;

 cleanup_name:
  free(name);

 cleanup_display:
  free(display.name);

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

  token_t* token;

  if ((token = parse_token(stream, &err)) == NULL)
    return err;

  if (! token_matches(token, "GrdL", TYPE_VAR_LEN_LIST))
    return GRD5_READ_PARSE;

  token_destroy(token);

  uint32_t ngrad;

  if ((err = parse_uint32(stream, &ngrad)) != GRD5_READ_OK)
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

      printf("  objc %*s %i\n", objc->namelen, objc->name, objc->value);

      objc_destroy(objc);

      /* gradient container */

      if ((token = parse_token(stream, &err)) == NULL)
	return err;

      if (! token_matches(token, "Grad", TYPE_OBJECT))
	return GRD5_READ_PARSE;

      token_destroy(token);

      /* inner objc */

      if ((objc = parse_objc(stream, &err)) == NULL)
	return err;

      uint32_t ncomp = objc->value;

      printf("  objc %*s %i\n", objc->namelen, objc->name, objc->value);

      objc_destroy(objc);

      switch (ncomp)
	{
	case 5:

	  // move to parse_gradient5()
	  
	  /* gradient title */
	  
	  if ((token = parse_token(stream, &err)) == NULL)
	    return err;

	  if (! token_matches(token, "Nm  ", TYPE_TEXT))
	    return GRD5_READ_PARSE;

	  token_destroy(token);
	  
	  uint32_t titlelen;

	  if ((err = parse_uint32(stream, &titlelen)) != GRD5_READ_OK)
	    return err;
	  else
	    {
	      char16_t title[titlelen];

	      if (fread(title, 2, titlelen, stream) != titlelen)
		return GRD5_READ_FREAD;

	      // add iconv magic here 
	    }
	  break;
	case 3:
	default:
	  return GRD5_READ_PARSE; 
	}
    }

  return GRD5_READ_OK;
}

