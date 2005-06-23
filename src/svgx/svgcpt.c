/*
  svgcpt.c : convert svg file to cpt file
 
  $Id: svgcpt.c,v 1.8 2005/06/22 23:08:05 jjg Exp jjg $
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

#include "svgcpt.h"

static int svgcpt_list(svgcpt_opt_t,svg_list_t*);
static int svgcpt_named(svgcpt_opt_t,svg_list_t*);
static int svgcpt_translate(svg_t*,cpt_t*);

extern int svgcpt(svgcpt_opt_t opt)
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

	  err += svgcpt_list(opt,list);
	  err += svgcpt_named(opt,list);
	}

      svg_list_destroy(list);
    }

  return err;
}

/* print the gradients in the list */ 

static int svg_id(svg_t*,const char*);

static int svgcpt_list(svgcpt_opt_t opt,svg_list_t* list)
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

static int svgcpt_named(svgcpt_opt_t opt,svg_list_t* list)
{
  svg_t* svg;
  cpt_t cpt;

  if (!opt.name) return 0;

  svg = svg_list_select(list,(int (*)(svg_t*,void*))svg_select_name,opt.name);

  if (!svg)
    {
      fprintf(stderr,"couldn't find gradient named %s\n",opt.name);
      return 1;
    }

  if (svgcpt_translate(svg,&cpt) != 0)
    {
      fprintf(stderr,"failed to convert %s to cpt\n",opt.name);
      return 1;
    }

  return 0;
}

static int svg_select_name(svg_t* svg,char* name)
{
  return (strcmp(svg->name,name) == 0 ? 1 : 0);
}

/* coonvert an svg_t to a cpt_t */

static int svgcpt_translate(svg_t* svg,cpt_t* cpt)
{
  return 1;
}
