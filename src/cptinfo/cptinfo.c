/*
  cptinfo.h - summary information on a cpt file
  J.J. Green 2004
  $Id$
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#include "cpt.h"
#include "colour.h"

#include "cptinfo.h"

typedef struct 
{
  int     size;
  model_t model;
  double  length;
  struct
  {
    int total;
    int hatch;
    int grey;
    int colour;
  } segments;
  struct
  {
    int discrete;
    int continuous;
  } type;
  struct
  {
    double min,max;
  } z;
} info_t;

#define NNSTR(x) ((x) ? (x) : NULL)

static int cptinfo_analyse(cpt_t*,info_t*);

static int output_plain(info_t,FILE*);
static int output_csv(info_t,FILE*);

extern int cptinfo(cptinfo_opt_t opt)
{
  FILE  *stream;
  cpt_t *cpt;
  info_t info;

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr,"failed to initialise cpt structure\n");
      return 1;
    }

  if (cpt_read(opt.file.input,cpt) != 0)
    {
      fprintf(stderr,"failed to read %s\n",NNSTR(opt.file.input));
      return 1;
    }

  if (cptinfo_analyse(cpt,&info) != 0)
    {
      fprintf(stderr,"failed to analyse %s\n",NNSTR(opt.file.input));
      return 1;
    }

  cpt_destroy(cpt);

  if (opt.file.output)
    {
      if ((stream = fopen(opt.file.output,"w")) == NULL)
	{
	  fprintf(stderr,
		  "failed to open %s : %s",
		  opt.file.output,
		  strerror(errno));
	  return 1;
	}
    }
  else
    stream = stdout;

  switch (opt.format)
    {
    case plain:
      if (output_plain(info,stream) != 0)
	{
	  fprintf(stderr,"error in plain output\n");
	  return 1;
	}
      break;

    case csv:
      if (output_csv(info,stream) != 0)
	{
	  fprintf(stderr,"error in csv output\n");
	  return 1;
	}
      break;

    case html:
      fprintf(stderr,"html not implemented yet\n");
      return 1;

   default:
      fprintf(stderr,"strange output specified\n");
      return 1;
    }

  fclose(stream);

  return 0;
}

static int analyse_segment(cpt_seg_t*,info_t*);

static int cptinfo_analyse(cpt_t* cpt,info_t* info)
{
  cpt_seg_t *s;

  /* zero the info counters */

  info->segments.total  = 0;
  info->segments.hatch  = 0;
  info->segments.grey   = 0;
  info->segments.colour = 0;

  /* set the model */

  info->model = cpt->model;

  /* the null gradient is vacuously discrete & continuous */

  info->type.discrete   = 1;
  info->type.continuous = 1;

  /* z-range */

  info->z.min = DBL_MAX;
  info->z.min = DBL_MIN;

  /* length */

  info->length = 0.0;

  s = cpt->segment;

  while (s)
    {
      if (analyse_segment(s,info) != 0)
	{
	  fprintf(stderr,"failed to analyse segment\n");
	  return 1;
	}

      s = s->rseg;
    }

  return 0;
}

static int analyse_segment(cpt_seg_t* seg,info_t* info)
{
  cpt_seg_t *right;
  double     len;

  info->segments.total++;  

  right = seg->rseg;

  if (right)
    {
      if (colour_rgb_dist(seg->rsmp.col,
			  right->lsmp.col,
			  info->model) > 0)
	  info->type.continuous = 0;
    }

  len = colour_rgb_dist(seg->rsmp.col,
			seg->lsmp.col,
			info->model);

  if (len > 0)
    info->type.discrete = 0;

  info->length += len;

  if (info->z.min > seg->lsmp.val) info->z.min = seg->lsmp.val;
  if (info->z.max < seg->rsmp.val) info->z.max = seg->rsmp.val;

  return 0;
}

#define BOOLSTR(x) ((x) ? "yes" : "no")

static int output_plain(info_t info,FILE* stream)
{
  const char *modstr;

  switch (info.model)
    {
    case rgb:
      modstr = "RGB";
      break;
    case hsv:
      modstr = "HSV";
      break;
    default:
      fprintf(stderr,"strange model\n");
      return 1;
    }
	
  fprintf(stream,"model:      %s\n",modstr);
  fprintf(stream,"continuous: %s\n",BOOLSTR(info.type.continuous));
  fprintf(stream,"discrete:   %s\n",BOOLSTR(info.type.discrete));
  fprintf(stream,"segments:\n");
  fprintf(stream,"  hatchure  %i\n",info.segments.hatch);
  fprintf(stream,"  greyscale %i\n",info.segments.grey);
  fprintf(stream,"  colour    %i\n",info.segments.colour);
  fprintf(stream,"  total     %i\n",info.segments.total);
  fprintf(stream,"z-range:    %.3f - %.3f\n",info.z.min,info.z.max);
  fprintf(stream,"rgb-length: %.3f\n",info.length);

  return 0;
}

static int output_csv(info_t info,FILE* stream)
{
  const char *modstr;

  switch (info.model)
    {
    case rgb:
      modstr = "rgb";
      break;
    case hsv:
      modstr = "hsv";
      break;
    default:
      fprintf(stderr,"strange model\n");
      return 1;
    }
	
  fprintf(stream,"%s,%s,%s,%i,%i,%i,%i,%.3f,%.3f,%.3f\n",
	  modstr,
	  BOOLSTR(info.type.continuous),
	  BOOLSTR(info.type.discrete),
	  info.segments.hatch,
	  info.segments.grey,
	  info.segments.colour,
	  info.segments.total,
	  info.z.min,
	  info.z.max,
	  info.length);

  return 0;
}
