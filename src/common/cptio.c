/*
  cptio.c

  read/write a cpt file
  (c) J.J Green 2004
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cptio.h"
#include "cptbridge.h"
#include "cptparse.h"
#include "cptscan.h"

/* 
   defined in cptparse.c but declared here -
   cptparse() is the main parser, cptdebug the
   parser debug flag
*/

extern int cptparse(void*);
extern int cptdebug;

/* utility defines */

#define LBUF 1024
#define SNM(x) ((x) ? (x) : "<stdin>")

extern int cpt_read(const char* file,cpt_t* cpt,int debug)
{
  FILE    *stream;
  yyscan_t cptscan;

  if (file)
    {
      const char *s;
      char *e,*name;

      if ((stream = fopen(file,"r")) == NULL)
	{
	  fprintf(stderr,"error reading %s : %s\n",SNM(file),strerror(errno));
	  return 1;
	}

      /* get the name from the filename */

      name = cpt->name;

      if ((s = strrchr(file, '/')) == NULL) 
	s = file;
      else
	s++;

      strncpy(name, s, CPT_NAME_LEN);

      /* chop off a trailing .cpt */

      if ((e = strrchr(name, '.')) != NULL)
	{   
	  if (strcmp(e,".cpt") == 0) *e = '\0';
	} 
    }
  else
    {
      stream = stdin;
      strncpy(cpt->name, "<stdin>", CPT_NAME_LEN);
    }

  /* 
     assign global cpt* acted upon by the bison parser, I'd like
     this to be an argument for cptparse, but bison only allows 
     one argument & that is used for the scanner
  */

  bridge = cpt;

  /*
    setup scanner
  */

  if (cptlex_init(&cptscan) != 0)
    {
      fprintf(stderr,"problem initailising scanner : %s\n",strerror(errno));
      return 1;
    }
  
  cptset_in(stream,cptscan);
  cptset_debug(debug,cptscan);

  /*
    do the parse
  */

  cptdebug = debug;

  if (cptparse(cptscan) != 0)
    {
      fprintf(stderr,"failed parse\n");
      return 1;
    }

  cptlex_destroy(cptscan);

  if (!feof(stream))
    {
      if (ferror(stream))
	{
	  fprintf(stderr,"error reading %s\n",SNM(file));
	  return 1;
	}
      else
	{
	  fprintf(stderr,"wierd error reading %s\n",SNM(file));
	}
    }

  if (stream != stdin) 
    fclose(stream);

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
