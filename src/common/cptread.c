/*
  cptread.c

  read a cpt file
  (c) J.J Green 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gmtcol.h"
#include "cptname.h"
#include "btrace.h"

#include "cptread.h"

static int cpt_parse(FILE *stream, cpt_t *cpt);

extern int cpt_read(const char *path, cpt_t *cpt)
{
  FILE *stream;
  int err = 1;

  if (path)
    {
      cpt->name = cptname(path, "cpt");

      if ((stream = fopen(path, "r")) == NULL)
	{
	  btrace("error reading %s : %s", path, strerror(errno));
	  return 1;
	}

      if ( !(err = cpt_parse(stream, cpt)) )
	{
	  if (! feof(stream) )
	    {
	      if (ferror(stream))
		{
		  btrace("error reading %s", path);
		  err = 1;
		}
	      else
		btrace("weird error reading %s", path);
	    }
	}
    }
  else
    err = cpt_parse(stdin, cpt);

  return err;
}

static void chomp(char *s)
{
  while (*s && *s != '\n' && *s != '\r')
    s++;

  *s = 0;
}

static int cpt_parse_comment(const char *line, model_t *model);
static int cpt_parse_global(const char *line, model_t model, fill_t *fill);
static int cpt_parse_segment(const char *line, cpt_t *cpt);

#define LINELEN 2048

static int cpt_parse(FILE *stream, cpt_t *cpt)
{
  int lineno;
  char line[LINELEN];

  for (lineno=1 ; fgets(line, LINELEN, stream) != NULL ; lineno++)
    {
      int err = 0;

      chomp(line);

      switch (line[0])
	{
	case '\0' :
	  break;
	case '#' :
	  err = cpt_parse_comment(line+1, &(cpt->model));
	  break;
	case 'B':
	  err = cpt_parse_global(line+1, cpt->model, &(cpt->bg));
	  break;
	case 'F':
	  err = cpt_parse_global(line+1, cpt->model, &(cpt->fg));
	  break;
	case 'N':
	  err = cpt_parse_global(line+1, cpt->model, &(cpt->nan));
	  break;
	default :
	  err = cpt_parse_segment(line, cpt);
	}

      if (err)
	{
	  btrace("parse error at line %i", lineno);
	  return 1;
	}
    }

  return 0;
}

static int cpt_parse_comment(const char *line, model_t *model)
{
  char mstr[5];

  if (sscanf(line, " COLOR_MODEL = %4s ", mstr) == 1)
    {
      if (strcmp(mstr, "RGB") == 0)
	*model = model_rgb;
      else if (strcmp(mstr, "HSV") == 0)
	*model = model_hsv;
      else if (strcmp(mstr, "+HSV") == 0)
	*model = model_hsvp;
      else
	{
	  btrace("colour model not handled: %s", mstr);
	  return 1;
	}
    }

  return 0;
}

static int cpt_parse_1fill(const char *s1, model_t model, fill_t *fill);
static int cpt_parse_3fill(const char *s1, const char *s2, const char *s3,
			   model_t model, fill_t *fill);

static int cpt_parse_global(const char *line, model_t model, fill_t *fill)
{
  /* make a copy of the line */

  size_t n = strlen(line);
  char buf[n+1];

  memcpy(buf, line, n+1);

  /* chop up the buffer */

  int ntok = 0;
  char *tok[3];

  if ((tok[ntok] = strtok(buf, " \t")) != NULL)
    {
      for (ntok=1 ;
	   (ntok < 3) && ((tok[ntok] = strtok(NULL, " \t")) != NULL) ;
	   ntok++);
    }

  int err = 0;

  switch (ntok)
    {
    case 1:
      err += cpt_parse_1fill(tok[0], model, fill);
      break;
    case 3:
      err += cpt_parse_3fill(tok[0], tok[1], tok[2], model, fill);
      break;
    default:
      btrace("fill with %i tokens", ntok);
      err++;
    }

  return (err ? 1 : 0);
}

static int cpt_parse_value(const char *str, double *val);
static int cpt_parse_annote(const char *str, annote_t *annote);

static int cpt_parse_segment(const char *line, cpt_t *cpt)
{
  /* the fresh segment */

  cpt_seg_t* seg;

  if (! (seg = cpt_seg_new()))
    {
      btrace("failed to create new segment");
      return 1;
    }

  /* make a copy of the line */

  size_t n = strlen(line);
  char buf[n+1];

  memcpy(buf, line, n+1);

  /* see if there is a label */

  char *label;

  if ((label = strrchr(buf, ';')) != NULL)
    {
      /* trim off the label */

      *label = '\0';

      /* skip leading whitespace */

      while (label++)
	{
	  int c = *label;

	  if (!c)
	    break;

	  if (!isspace(c))
	    {
	      /* copy the label into the segment */
	      seg->label = strdup(label);
	      break;
	    }
	}
    }

  /* chop up the buffer */

  int ntok = 0;
  char *tok[9];

  if ((tok[ntok] = strtok(buf, " \t")) != NULL)
    {
      for (ntok = 1 ;
	   (ntok < 9) && ((tok[ntok] = strtok(NULL, " \t")) != NULL) ;
	   ntok++);
    }

  /* interpet the pieces */

  int err = 0;

  switch (ntok)
    {
    case 9 :
      err += cpt_parse_annote(tok[8], &(seg->annote));
    case 8 :
      err += cpt_parse_value(tok[0], &(seg->lsmp.val));
      err += cpt_parse_3fill(tok[1], tok[2], tok[3],
			     cpt->model, &(seg->lsmp.fill));
      err += cpt_parse_value(tok[4], &(seg->rsmp.val));
      err += cpt_parse_3fill(tok[5], tok[6], tok[7],
			     cpt->model, &(seg->rsmp.fill));
      break;

    case 5 :
      err += cpt_parse_annote(tok[4], &(seg->annote));
    case 4 :
      err += cpt_parse_value(tok[0], &(seg->lsmp.val));
      err += cpt_parse_1fill(tok[1], cpt->model, &(seg->lsmp.fill));
      err += cpt_parse_value(tok[2], &(seg->rsmp.val));
      err += cpt_parse_1fill(tok[3], cpt->model, &(seg->rsmp.fill));
      break;

    default :
      btrace("segment with strange number of tokens (%i)", ntok);
      return 1;
    }

  if (err) return 1;

  return cpt_append(seg, cpt);
}

static int cpt_parse_value(const char *str, double *val)
{
  *val = atof(str);
  return 0;
}

static int cpt_parse_annote(const char *str, annote_t *annote)
{
  switch (str[0])
    {
    case 'L' : *annote = lower; break;
    case 'U' : *annote = upper; break;
    case 'B' : *annote = both;  break;
    default:
      btrace("strange annotation: %s", str);
      return 1;
    }

  return 0;
}

static int set_hsv(double h, double s, double v, hsv_t *hsv);
static int set_rgb(int r, int g, int b, rgb_t *rgb);

static int cpt_parse_1fill(const char *str, model_t model, fill_t *fill)
{
  /* empty */

  if (strcmp(str, "-") == 0)
    {
      fill->type = fill_empty;
      return 0;
    }

  /* standard GMT colour */

  const struct gmtcol_t *gc;

  if ((gc = gmtcol(str)) != NULL)
    {
      if (model != model_rgb)
	{
	  btrace("RGB colour %s with non-RGB colour model", str);
	  return 1;
	}

      fill->type = fill_colour;
      fill->u.colour.rgb.red   = gc->r;
      fill->u.colour.rgb.green = gc->g;
      fill->u.colour.rgb.blue  = gc->b;

      return 0;
    }

  /* hatch */

  hatch_t hatch;

  if (sscanf(str, "p%i/%i", &(hatch.dpi), &(hatch.n)) == 2)
    {
      hatch.sign = 1;
      fill->type = fill_hatch;
      fill->u.hatch = hatch;
      return 0;
    }

  if (sscanf(str, "P%i/%i", &(hatch.dpi), &(hatch.n)) == 2)
    {
      hatch.sign = -1;
      fill->type = fill_hatch;
      fill->u.hatch = hatch;
      return 0;
    }

  /* handle r/g/b or h/s/v strings */

  switch (model)
    {
    case model_rgb:
      {
	int r, g, b;

	if (sscanf(str, "%d/%d/%d", &r, &g, &b) == 3)
	  return set_rgb(r, g, b, &(fill->u.colour.rgb));
      }
      break;

    case model_hsv:
    case model_hsvp:
      {
	double h, s, v;

	if (sscanf(str, "%lf/%lf/%lf", &h, &s, &v) == 3)
	  return set_hsv(h, s, v, &(fill->u.colour.hsv));
      }
      break;

    default:
      return 1;
    }

  /* integer (greyscale) */

  char *endptr;
  long val;

  val = strtol(str, &endptr, 10);

  if (endptr != str)
    {
      if ((val < 0) || (val > 255))
	{
	  btrace("integer %li outside range 0,...,255", val);
	  return 1;
	}

      fill->type   = fill_grey;
      fill->u.grey = val;

      return 0;
    }

  btrace("fill not recognised: %s", str);

  return 1;
}

static int cpt_parse_3fill(const char *s1,
			   const char *s2,
			   const char *s3,
			   model_t model,
			   fill_t *fill)
{
  int err = 0;

  fill->type = fill_colour;

  switch (model)
    {
    case model_rgb:
      err += set_rgb(atoi(s1), atoi(s2), atoi(s3), &(fill->u.colour.rgb));
      break;
    case model_hsv:
    case model_hsvp:
      err += set_hsv(atof(s1), atof(s2), atof(s3), &(fill->u.colour.hsv));
      break;
    default:
      btrace("bad fill type");
      err++;
    }

  return err;
}

static int set_rgb(int r, int g, int b, rgb_t *rgb)
{
  if ((r < 0) || (r > 255))
    {
      btrace("red %i out of range", r);
      return 1;
    }

  if ((g < 0) || (g > 255))
    {
      btrace("green %i out of range", g);
      return 1;
    }

  if ((b < 0) || (b > 255))
    {
      btrace("blue %i out of range", b);
      return 1;
    }

  rgb->red   = r;
  rgb->green = g;
  rgb->blue  = b;

  return 0;
}

static int set_hsv(double h, double s, double v, hsv_t *hsv)
{
  if ((h < 0.0) || (h > 360.0))
    {
      btrace("hue %.3f out of range", h);
      return 1;
    }

  if ((s < 0.0) || (s > 1.0))
    {
      btrace("saturation %.3f out of range", s);
      return 1;
    }

  if ((v < 0.0) || (v > 1.0))
    {
      btrace("value %.3f out of range", v);
      return 1;
    }

  hsv->hue = h;
  hsv->sat = s;
  hsv->val = v;

  return 0;
}
