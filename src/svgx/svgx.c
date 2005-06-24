/*
  svgx.c : convert svg file to cpt file
 
  $Id: svgx.c,v 1.9 2005/06/23 23:13:56 jjg Exp jjg $
  J.J. Green 2005
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "colour.h"

#include "svgread.h"

#include "cpt.h"
#include "cptio.h"

#include "svgx.h"

static int svgx_list(svgx_opt_t,svg_list_t*);
static int svgx_named(svgx_opt_t,svg_list_t*);
static int svgx_all(svgx_opt_t,svg_list_t*);

static int svgcpt(svg_t*,cpt_t*);

extern int svgx(svgx_opt_t opt)
{
  int err = 0;
  svg_list_t *list;

  if ((list = svg_list_new()) == NULL)
    {
      fprintf(stderr,"error creating new list\n");
      err++;
    }
  else
    {
      if (svg_read(opt.input.file,list) != 0)
	{
	  fprintf(stderr,"error reading svg\n");
	  err++;
	}
      else
	{
	  /* run the job */

	  err += svgx_list(opt,list);
	  err += svgx_named(opt,list);
	  err += svgx_all(opt,list);
	}

      svg_list_destroy(list);
    }

  return err;
}

/* print the gradients in the list */ 

static int svg_id(svg_t*,const char*);

static int svgx_list(svgx_opt_t opt,svg_list_t* list)
{
  int n,err=0;

  if (!opt.list) return 0;

  n = svg_list_size(list);

  if (opt.verbose)
    {
      if (n==0)
	{
	  /* worrying, but not an error */

	  printf("no gradient found!\n");
	}
      else
	{
	  printf("found %i gradient%s\n",n,(n==1 ? "" : "s"));
	  err = svg_list_iterate(list,(int (*)(svg_t*,void*))svg_id,"  %s\n");
	}
    }
  else
    {
      err = svg_list_iterate(list,(int (*)(svg_t*,void*))svg_id,"%s\n");
    }

  if (err)  fprintf(stderr,"error listing svg\n");

  return err;
}

static int svg_id(svg_t* svg,const char* fmt)
{
  printf(fmt,svg->name);
  return 0;
}

/* convert a named gradient */

static int svg_select_name(svg_t*,char*);

static int svgx_named(svgx_opt_t opt,svg_list_t* list)
{
  svg_t *svg;
  cpt_t *cpt;
  char *file;

  /* get svg with this name */

  if (!opt.name) return 0;

  svg = svg_list_select(list,(int (*)(svg_t*,void*))svg_select_name,opt.name);

  if (!svg)
    {
      fprintf(stderr,"couldn't find gradient named %s\n",opt.name);
      return 1;
    }

  file = opt.output.file;

  switch (opt.type)
    {
    case type_cpt:

      if ((cpt = cpt_new()) == NULL)
	{
	  fprintf(stderr,"failed to create cpt structure\n");
	  return 1;
	}
      
      if (svgcpt(svg,cpt) != 0)
	{
	  fprintf(stderr,"failed to convert %s to cpt\n",opt.name);
	  return 1;
	}
            
      if (cpt_write(file,cpt) != 0)
	{
	  fprintf(stderr,"failed to write to %s\n",(file ? file : "<stdout>"));
	  return 1;
	}
      
      cpt_destroy(cpt);

      break;

    case type_ggr:

      fprintf(stderr,"ggr output not yet supported\n");
      return 1;

    default:

      fprintf(stderr,"strange output format!\n");
      return 1;
    }

  if (opt.verbose)
    printf("wrote %s to %s\n",opt.name,(file ? file : "<stdout>"));

  return 0;
}

static int svg_select_name(svg_t* svg,char* name)
{
  return (strcmp(svg->name,name) == 0 ? 1 : 0);
}

static int svgcpt_dump(svg_t*,svgx_opt_t*);

static int svgx_all(svgx_opt_t opt,svg_list_t* list)
{
  int (*dump)(svg_t*,void*);

  if (!opt.all) return 0;

  switch (opt.type)
    {
    case type_cpt:
      dump = (int (*)(svg_t*,void*))svgcpt_dump;
      break;

    case type_ggr:
      fprintf(stderr,"ggr not working yet!\n");
      return 1;

    default:

      fprintf(stderr,"strange output format!\n");
      return 1;
    }

  if (opt.verbose)
    printf("converting all gradients:\n");

  if (svg_list_iterate(list,dump,&opt) != 0)
    {
      fprintf(stderr,"failed writing all gradients\n");
      return 1;
    }

  return 0;
}

static int svgcpt_dump(svg_t* svg,svgx_opt_t* opt)
{
  int  n = SVG_NAME_LEN+5;
  char file[n],*name;
  cpt_t *cpt;

  if (!svg) return 1;

  name = svg->name;

  if (snprintf(file,n,"%s.cpt",name) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr,"failed to create cpt structure\n");
      return 1;
    }

  /* translate */

  if (svgcpt(svg,cpt) != 0)
    {
      fprintf(stderr,"failed to convert %s to cpt\n",name);
      return 1;
    }

  /* write */

  if (cpt_write(file,cpt) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  cpt_destroy(cpt);

  if (opt->verbose)  
    printf("  %s\n",file);

  return 0;
}

/* coonvert an svg_t to a cpt_t */

static int svgcpt(svg_t* svg,cpt_t* cpt)
{
  svg_node_t *node,*next;
  fill_t fill;

  cpt->model = rgb;

  fill.type = rgb;
  fill.u.colour.rgb.red   = 255;
  fill.u.colour.rgb.green = 255;
  fill.u.colour.rgb.blue  = 255;

  cpt->bg  = fill;
  cpt->fg  = fill;
  cpt->nan = fill;

  if (snprintf(cpt->name,CPT_NAME_LEN,"%s",svg->name) >= CPT_NAME_LEN)
    {
      fprintf(stderr,"cpt name truncated\n");
    }

  node = svg->nodes;
  next = node->r; 

  while (next)
    {
      double z1,z2;

      z1 = node->stop.value;
      z2 = next->stop.value;

      if (z1 < z2)
	{
	  rgb_t c1,c2;
	  cpt_seg_t* seg;

	  c1 = node->stop.colour;
	  c2 = next->stop.colour;

	  if ((seg = cpt_seg_new()) == NULL)
	    {
	      fprintf(stderr,"failed to create cpt segment\n");
	      return 1;
	    }

	  seg->lsmp.val = z1;
	  seg->rsmp.val = z2;

	  seg->lsmp.fill.type         = colour;
	  seg->lsmp.fill.u.colour.rgb = c1;

	  seg->rsmp.fill.type         = colour;
	  seg->rsmp.fill.u.colour.rgb = c2;

	  if (cpt_append(seg,cpt) != 0)
	    {
	      fprintf(stderr,"failed to append segment\n");
	      return 1;
	    }
	}

      node = next;
      next = node->r;
    } 

  return 0;
}
