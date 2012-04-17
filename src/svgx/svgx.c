/*
  svgx.c : convert svg to other formats
 
  $Id: svgx.c,v 1.36 2012/04/17 17:31:20 jjg Exp jjg $
  J.J. Green 2005, 2011
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "colour.h"
#include "svgread.h"

#include "svgx.h"
#include "svgxdump.h"

static int svgx_list(svgx_opt_t, svg_list_t*);
static int svgx_first(svgx_opt_t, svg_list_t*);
static int svgx_named(svgx_opt_t, svg_list_t*);
static int svgx_all(svgx_opt_t, svg_list_t*);

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
      if (svg_read(opt.input.file, list) != 0)
	{
	  fprintf(stderr,"error reading svg\n");
	  err++;
	}
      else
	{
	  switch (opt.job)
	    {
	    case job_list:
	      err = svgx_list(opt,list);
	      break;
	      
	    case job_first:
	      err = svgx_first(opt,list);
	      break;

	    case job_named:
	      err = svgx_named(opt,list);
	      break;
	      
	    case job_all:
	      err = svgx_all(opt,list);
	      break;

	    default:
	      return 1;
	    }
	}

      svg_list_destroy(list);
    }

  return err;
}

/* print the gradients in the list */ 

static int svg_id(svg_t *svg, const char* fmt)
{
  printf(fmt,svg->name);
  return 0;
}

static int svgx_list(svgx_opt_t opt, svg_list_t *list)
{
  int n,err=0;

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

  if (err)  
    fprintf(stderr,"error listing svg\n");

  return err;
}

static int svg_select_name(svg_t*, char*);
static int svg_select_first(svg_t*, char*);
static int svgx_single(svgx_opt_t, svg_t*);

static int svgx_first(svgx_opt_t opt, svg_list_t *list)
{
  svg_t *svg;

  if ((svg = svg_list_select(list,
			     (int (*)(svg_t*,void*))svg_select_first,
			     NULL)) == NULL)
    {
      fprintf(stderr,"couldn't find first gradient!\n");
      return 1;
    }

  return svgx_single(opt, svg);
}

static int svgx_named(svgx_opt_t opt,svg_list_t *list)
{
  svg_t *svg;

  if ((svg = svg_list_select(list,
			     (int (*)(svg_t*,void*))svg_select_name,
			     opt.name)) == NULL)
    {
      fprintf(stderr,"couldn't find gradient named %s\n",opt.name);
      return 1;
    }

  return svgx_single(opt, svg);
}

/* return whether a particular type should be flattened */

static int flatten_type(svgx_type_t type)
{
  switch (type)
    {
    case type_cpt:
    case type_sao:
    case type_gpt:
      return 1;
    case type_ggr:
    case type_pov:
    case type_css3:
    case type_psp:
    case type_png:
    case type_svg:
      return 0;
    default:
      fprintf(stderr,"strange type\n");
      return -1;
    }
}

/* return dump function for a particular type */

typedef int (*dump_f)(const svg_t*, svgx_opt_t*);

static dump_f dump_type(svgx_type_t type)
{
  dump_f dump;

  switch (type)
    {
    case type_cpt:  dump = svgcpt_dump;  break;
    case type_ggr:  dump = svgggr_dump;  break;
    case type_psp:  dump = svgpsp_dump;  break;
    case type_pov:  dump = svgpov_dump;  break;
    case type_gpt:  dump = svggpt_dump;  break;
    case type_css3: dump = svgcss3_dump; break;
    case type_sao:  dump = svgsao_dump;  break;
    case type_png:  dump = svgpng_dump;  break;
    case type_svg:  dump = svgsvg_dump;  break;
 
    default:

      fprintf(stderr,"strange output format!\n");
      dump = NULL;
    }
  
  return dump;
}

/* write a single file */

static int svgx_single(svgx_opt_t opt, svg_t *svg)
{
  const char *file;

  if (svg_explicit(svg) != 0)
    {
      fprintf(stderr,"failed adding explicit stops\n");
      return 1;
    }

  file = opt.output.file;

  if (flatten_type(opt.type))
    {
      if (svg_flatten(svg, opt.format.alpha) != 0)
	{
	  fprintf(stderr,"failed to flatten transparency\n");
	  return 1;
	}
    }

  dump_f dump;

  if ((dump = dump_type(opt.type)) == NULL)
    return 1;

  if (dump(svg,&opt) != 0)
    {
      fprintf(stderr,"failed conversion\n");
      return 1;
    }

  if (opt.verbose)
    printf("wrote %s to %s\n",
	   (opt.name ? opt.name : "gradient"),
	   (file ? file : "<stdout>"));
  
  return 0;
}

static int svg_select_first(svg_t *svg, char* name)
{
  return 1;
}

static int svg_select_name(svg_t *svg, char* name)
{
  return (strcmp((const char*)svg->name, name) == 0 ? 1 : 0);
}

static int svg_explicit2(svg_t *svg, void *dummy)
{
  return svg_explicit(svg);
}

static int svg_flatten2(svg_t *svg, rgb_t *alpha)
{
  return svg_flatten(svg,*alpha);
}

static int svgx_all(svgx_opt_t opt, svg_list_t *list)
{
  /* coerce explicit */

  if (svg_list_iterate(list, svg_explicit2, NULL) != 0)
    {
      fprintf(stderr,"failed coerce explicit\n");
      return 1;
    }

  /* coerce flat */

  if (flatten_type(opt.type))
    {
      if (svg_list_iterate(list, 
			   (int (*)(svg_t*, void*))svg_flatten2, 
			   &(opt.format.alpha)) != 0)
	{
	  fprintf(stderr,"failed coerce explicit\n");
	  return 1;
	}
    }

  int (*dump)(svg_t*, void*);

  if ((dump = (int (*)(svg_t*, void*))dump_type(opt.type)) == NULL) 
    return 1;

  if (opt.verbose)
    printf("converting all gradients:\n");

  if (svg_list_iterate(list, dump, &opt) != 0)
    {
      fprintf(stderr,"failed writing all gradients\n");
      return 1;
    }

  return 0;
}
