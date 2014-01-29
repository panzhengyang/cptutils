/*
  grd5read.c
  Copyright (c) J.J. Green 2014
*/

#include "grd5read.h"
#include "grd5type.h"
#include "htons.h"

#include <stdio.h>
#include <string.h>

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

static int parse_varlist(FILE *stream, const char *expected, uint32_t *count)
{
  int err;
  uint32_t len;

  if ((err = parse_uint32(stream, &len)) != GRD5_READ_OK)
    return err;

  if (len == 0) len = 4;

  char name[len];

  if (fread(&name, 1, len, stream) != len)
    return GRD5_READ_FREAD;

  if (expected && (strncmp(name, expected, len) != 0))
    return GRD5_READ_PARSE;

  int type;

  if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
    return err;

  if (type != TYPE_VAR_LEN_LIST)
    return GRD5_READ_PARSE;

  if ((err = parse_uint32(stream, count)) != GRD5_READ_OK)
    return err;

  return GRD5_READ_OK;
}

static int parse_objc(FILE *stream, const char *expected, uint32_t *count)
{
  int err;
  uint32_t len;

  if ((err = parse_uint32(stream, &len)) != GRD5_READ_OK)
    return err;
  else
    {
      /*
	this is typically what looks like a wide-character string
	of "gradient", we ignore this for now ...
      */

      len *= 2;

      char name[len];

      if (fread(&name, 1, len, stream) != len)
	return GRD5_READ_FREAD;
    }

  if ((err = parse_uint32(stream, &len)) != GRD5_READ_OK)
    return err;
  else
    {
      char name[len];

      if (fread(&name, 1, len, stream) != len)
	return GRD5_READ_FREAD;

      if (expected && (strncmp(name, expected, len) != 0))
	return GRD5_READ_PARSE;

      if ((err = parse_uint32(stream, &len)) != GRD5_READ_OK)
	return err;
    }

  if ((err = parse_uint32(stream, count)) != GRD5_READ_OK)
    return err;

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

  uint32_t ngrad;

  err = parse_varlist(stream, "GrdL", &ngrad);

  printf("%i gradients\n", ngrad);

  int i;
  
  for (i=0 ; i<ngrad ; i++)
    {
      int type;
      uint32_t count;

      /* seemingly pointless 1-element container */

      if ((err = parse_type(stream, &type)) != GRD5_READ_OK)
	return err;

      if (type != TYPE_OBJECT) return GRD5_READ_PARSE;

      if ((err = parse_objc(stream, "Grdn", &count)) != GRD5_READ_OK)
	return err;

      if (count != 1) return GRD5_READ_PARSE;

      /* gradient container */

      // FIXME

    }

  return GRD5_READ_OK;
}

