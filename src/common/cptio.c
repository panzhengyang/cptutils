/*
  cptio.c

  read/write a cpt file
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

#include "cptio.h"

#include "gmtcol.h"

static int cpt_parse(FILE *stream, cpt_t *cpt);

extern int cpt_read(const char *file, cpt_t *cpt)
{
  FILE *stream;
  int err = 1;

  if (file)
    {
      if ((stream = fopen(file, "r")) == NULL)
	{
	  fprintf(stderr,"error reading %s : %s\n", file, strerror(errno));
	  return 1;
	}
      
      if ( !(err = cpt_parse(stream, cpt)) )
	{
	  if (! feof(stream) )
	    {
	      if (ferror(stream))
		{
		  fprintf(stderr,"error reading %s\n", file);
		  err = 1;
		}
	      else
		fprintf(stderr,"weird error reading %s\n", file);
	    }
	}
    }
  else
    err = cpt_parse(stdin, cpt);

  return err;
}

static void chomp(char *str) 
{
  char *p = strrchr(str, '\n');
  if (p) *p = '\0';
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
	  fprintf(stderr, "parse error at line %i\n", lineno);
	  fprintf(stderr, "%s\n", line);
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
      else
	{
	  fprintf(stderr, "colour model not handled: %s\n", mstr);
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

  for (tok[ntok] = strtok(buf, " \t") ;
       ntok < 3 && tok[ntok] ;
       tok[++ntok] = strtok(NULL, " \t"));

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
      fprintf(stderr, "fill with %i tokens\n", ntok);
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
      fprintf(stderr, "failed to create new segment\n");
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

  for (tok[ntok] = strtok(buf, " \t") ;
       ntok < 9 && tok[ntok] ;
       tok[++ntok] = strtok(NULL, " \t"));

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
      fprintf(stderr, "segment with strange number of tokens (%i)\n", ntok);
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
      fprintf(stderr, "strange annotation: %s\n", str);
      return 1;
    }

  return 0;
}

static int cpt_parse_1fill(const char *str, model_t model, fill_t *fill)
{
  /* empty */

  if (strcmp(str, "-") == 0)
    {
      fill->type = fill_empty;
      return 0;
    }

  /* standard GMT colour */

  struct gmtcol_t *gc;

  if ((gc = gmtcol(str)) != NULL)
    {
      if (model != model_rgb)
	{
	  fprintf(stderr, "RGB colour %s with non-RGB colour model\n", str); 
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

  // FIXME, handle r/g/b triples

  /* integer (greyscale) */

  char *endptr;
  long val;
  
  val = strtol(str, &endptr, 10);

  if (endptr != str)
    {
      if ((val < 0) || (val > 255))
	{
	  fprintf(stderr, "integer %li outside range 0,...,255\n", val);
	  return 1;
	}

      fill->type   = fill_grey;
      fill->u.grey = val;

      return 0;
    }

  fprintf(stderr, "fill not recognised: %s\n", str);
  return 1;
}

static int cpt_parse_3fill(const char *s1, 
			   const char *s2, 
			   const char *s3,
			   model_t model,
			   fill_t *fill)
{
  fill->type = fill_colour;

  switch (model)
    {
    case model_rgb:
      fill->u.colour.rgb.red   = atoi(s1);
      fill->u.colour.rgb.green = atoi(s2);
      fill->u.colour.rgb.blue  = atoi(s3);
      break;
    case model_hsv:
      fill->u.colour.hsv.hue = atof(s1);
      fill->u.colour.hsv.sat = atof(s2);
      fill->u.colour.hsv.val = atof(s3);
      break;
    default:
      fprintf(stderr, "bad colour model\n");
      return 1;
    }

  return 0;
}

static int fprintf_cpt_aux(FILE*,char,fill_t,model_t);
static int fprintf_cpt_sample(FILE*, cpt_sample_t, model_t, const char*);
static int common_format(cpt_t *cpt, char *fmt);

#define ZFMTLEN 64

extern int cpt_write(const char *outfile, cpt_t *cpt)
{
  FILE       *stream;
  cpt_seg_t  *seg;
  const char *model;
  time_t      t;
  char        zfmt[ZFMTLEN];
  
  if (common_format(cpt, zfmt) != 0)
    {
      fprintf(stderr,"failed to build z-format string\n");
      return 1;
    }

  if (outfile)
    {
      stream = fopen(outfile,"wb");
      if (!stream) return 1;
    }
  else stream = stdout;
  
  t = time(NULL);
  
  switch (cpt->model)
    {
    case model_rgb : model = "RGB"; break;
    case model_hsv : model = "HSV"; break;
    default :
      fprintf(stderr,"cpt file corrupt : bad model specified\n");
      return 1;
    }
  
  if (outfile)  fprintf(stream, "# %s\n", outfile);
  
  fprintf(stream,"# autogenerated GMT palette");
  if (strlen(cpt->name) > 0)
    fprintf(stream," \"%s\"", cpt->name);
  fprintf(stream,"\n");
  fprintf(stream,"# cptutils version %s, %s",VERSION,ctime(&t));
  fprintf(stream,"# COLOR_MODEL = %s\n",model);
  
  seg = cpt->segment;
  
  /* needs to write annotation too */
  
  while (seg)
    {
      fprintf_cpt_sample(stream, seg->lsmp, cpt->model, zfmt);
      fprintf(stream," ");
      
      fprintf_cpt_sample(stream, seg->rsmp, cpt->model, zfmt);
      if (seg->label) fprintf(stream, " ; %s", seg->label);
      fprintf(stream,"\n");
      
      seg = seg->rseg;
    }
  
  fprintf_cpt_aux(stream, 'B', cpt->bg,  cpt->model);
  fprintf_cpt_aux(stream, 'F', cpt->fg,  cpt->model);
  fprintf_cpt_aux(stream, 'N', cpt->nan, cpt->model);
  
  fclose(stream);
  
  return 0;
}

/*
  Given the first sample of a cpt, evaluate a suitable
  format string for all of the z-values, some heuristics
  are used here.
*/

#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)<(b)) ? (a) : (b))

#define WRATIO_MAX 100.0
#define WIDTH_MAX  1e5
#define WIDTH_MIN  1e-5
#define SIGFIG_MIN 5

static int dpused(const char *str, size_t* dpu)
{
  char *c;

  if ( (c = strchr(str,'.')) == NULL) return 1;

  size_t i,len = strlen(c);

  if (len < 2) return 1;

  for (i=len-1 ; (i > 0) && (c[i] == '0')  ; i--);

  *dpu = i;

  return 0;
}


static int common_format(cpt_t *cpt, char *fmt)
{
  /* create array of z-values */

  size_t i, nseg = cpt_nseg(cpt), nstop = nseg+1;
  
  if (nstop < 2)
    {
      fprintf(stderr,"empty cpt file\n");
      return 1;
    }

  double z[nstop];
  cpt_seg_t *seg = cpt->segment;

  z[0] = seg->lsmp.val;
      
  for (i=0 ; i<nseg ; i++)
    {
      z[i+1] = seg->rsmp.val;
      seg = seg->rseg;
    }

  /* get min/max widths */

  double wmin = fabs(z[0]-z[1]), wmax = wmin;

  for (i=1 ; i<nstop-1 ; i++)
    {
      double w = fabs(z[i]-z[i+1]);
      wmax = MAX(w,wmax);
      wmin = MIN(w,wmin);
    }

  /*
    if width is large/small or ratio is large
    then fall back to exponential format
  */

  if (
      (wmax > WRATIO_MAX * wmin) ||
      (wmax > WIDTH_MAX) || 
      (wmin < WIDTH_MIN)
      )
    return (snprintf(fmt,ZFMTLEN,"%%7e ") >= ZFMTLEN ? 1 : 0);

  /*
    default number of dp - SIGFIG_MIN orders
    of magnitude less than wmin
  */

  int dp = ceil(SIGFIG_MIN - log10(wmin));

  dp = MAX(dp,0);

  if (dp > 0)
    {
      /* see how many dp we acually need */

      size_t maxdpu = 0;

      for (i=0 ; i<nstop ; i++)
	{
	  if (snprintf(fmt,ZFMTLEN,"%.*f",dp,z[i]) >= ZFMTLEN)
	    return 1;

	  size_t dpu = 0;

	  if ( dpused(fmt,&dpu) != 0 )
	    {
	      fprintf(stderr,"dpused fail");
	      return 1;
	    }

	  maxdpu = MAX(maxdpu,dpu);
	}

      dp = maxdpu;
    }

  /* find the maximal width used with this dp */

  size_t maxlen = 0;

  for (i=0 ; i<nstop ; i++)
    {
      if (snprintf(fmt,ZFMTLEN,"%.*f", dp, z[i]) >= ZFMTLEN)
	return 1;

      size_t len = strlen(fmt);
      maxlen = MAX(len, maxlen);
    }

  /* generate the format string */

  if (snprintf(fmt,ZFMTLEN,"%%%zi.%df ", maxlen, dp) >= ZFMTLEN)
    return 1;

  return 0;
}

static int fprintf_cpt_fill(FILE*,fill_t,model_t);

static int fprintf_cpt_sample(FILE* stream, cpt_sample_t smp, 
			      model_t model, const char *fmt)
{
  int n = 0;

  n += fprintf(stream,fmt,smp.val);
  n += fprintf_cpt_fill(stream,smp.fill,model);

  return n;
}

static int fprintf_cpt_fill(FILE* stream,fill_t fill,model_t model)
{
  int n = 0;

  switch (fill.type)
    {
    case fill_empty:
      n += fprintf(stream,"-");
      break;
    case fill_grey:
      n += fprintf(stream,"%3i",fill.u.grey);
      break;
    case fill_hatch:
      n += fprintf(stream,"%c%i/%i",
		   (fill.u.hatch.sign == 1 ? 'p' : 'P' ), 
		   fill.u.hatch.dpi,
		   fill.u.hatch.n
		   );
      break;
    case fill_file:
      n += fprintf(stream,"%s",fill.u.file);
      break;
    case fill_colour:
      switch (model)
	{
	case model_rgb:
	  n += fprintf(stream,
		       "%3i %3i %3i",
		       fill.u.colour.rgb.red,
		       fill.u.colour.rgb.green,
		       fill.u.colour.rgb.blue);
	    break;
	case model_hsv:
	  n += fprintf(stream,
		       "%7.3f %7.5f %7.5f",
		       fill.u.colour.hsv.hue,
		       fill.u.colour.hsv.sat,
		       fill.u.colour.hsv.val);
	  break;
	default:
	  break;
	}
    }
  
  return n;
}

static int fprintf_cpt_aux(FILE* stream, char c, fill_t fill, model_t model)
{
  int n = 0;

  if (fill.type == fill_empty)
    return 0;

  n += fprintf(stream,"%c ",c);
  n += fprintf_cpt_fill(stream,fill,model);
  n += fprintf(stream,"\n");

  return n;
}
