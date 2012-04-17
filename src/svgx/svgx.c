/*
  svgx.c : convert svg to other formats
 
  $Id: svgx.c,v 1.35 2012/04/17 11:19:35 jjg Exp jjg $
  J.J. Green 2005, 2011
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "colour.h"
#include "svgread.h"

#include "cpt.h"
#include "cptio.h"
#include "ggr.h"
#include "povwrite.h"
#include "gptwrite.h"
#include "css3write.h"
#include "pspwrite.h"
#include "saowrite.h"
#include "pngwrite.h"
#include "svgwrite.h"

#include "svgx.h"
#include "utf8x.h"

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
	  /* run the job */

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


/* convert a named gradient */

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

/* the dump function */

static int svgcpt_dump(const svg_t*, svgx_opt_t*);
static int svgggr_dump(const svg_t*, svgx_opt_t*);
static int svgpov_dump(const svg_t*, svgx_opt_t*);
static int svggpt_dump(const svg_t*, svgx_opt_t*);
static int svgcss3_dump(const svg_t*, svgx_opt_t*);
static int svgpsp_dump(const svg_t*, svgx_opt_t*);
static int svgsao_dump(const svg_t*, svgx_opt_t*);
static int svgpng_dump(const svg_t*, svgx_opt_t*);
static int svgsvg_dump(const svg_t*, svgx_opt_t*);

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

/*
  create a new opt with an autogenerated filename and call 
  f(svg,opt), this is common code for the dump function in
  the case opt.job == job_all;
*/

static int call_autonamed(const svg_t *svg, 
			  svgx_opt_t *opt, 
			  const char *suffix,
			  int (*f)(const svg_t*, svgx_opt_t*))
{
  const char *name = (char*)svg->name;
  int  n = strlen(name) + strlen(suffix) + 2;
  char file[n];

  if (snprintf(file, n, "%s.%s", name, suffix) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  svgx_opt_t opt2 = *opt;

  opt2.job = job_named;
  opt2.output.file = file;

  int err = f(svg, &opt2);

  if (opt->verbose) printf("  %s\n",file);

  return err;
}

/* conversion function */

static int svgcpt(const svg_t*, cpt_t*);
static int svgggr(const svg_t*, gradient_t*);
static int svgpsp(const svg_t*, psp_t*);
static int svgpov(const svg_t*, pov_t*);
static int svggpt(const svg_t*, gpt_t*);
static int svgcss3(const svg_t*, css3_t*);
static int svgsao(const svg_t*, sao_t*);
static int svgpng(const svg_t*, png_t*);

/* check functions */

static int svgpov_valid(const svg_t*, int, int);

/* dump functions */

static int svgcpt_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "cpt", svgcpt_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  cpt_t *cpt;

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr,"failed to create cpt structure\n");
      return 1;
    }

  cpt->model = model_rgb;
  
  cpt->fg.type = cpt->bg.type = cpt->nan.type = fill_colour;
  
  cpt->bg.u.colour.rgb  = opt->format.cpt.bg;
  cpt->fg.u.colour.rgb  = opt->format.cpt.fg;
  cpt->nan.u.colour.rgb = opt->format.cpt.nan;
  
  if (snprintf(cpt->name,CPT_NAME_LEN,"%s",name) >= CPT_NAME_LEN)
    {
      fprintf(stderr,"gradient name (%s) truncated\n",name);
    }
  
  if (svgcpt(svg,cpt) != 0)
    {
      fprintf(stderr,"failed to convert %s to cpt\n",name);
      return 1;
    }

  if (cpt_write(file,cpt) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  cpt_destroy(cpt);

  return 0;
}

static int svgggr_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "ggr", svgggr_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  gradient_t *ggr;

  if ((ggr = grad_new_gradient()) == NULL)
    {
      fprintf(stderr,"failed to create ggr structure\n");
      return 1;
    }

  if (svgggr(svg,ggr) != 0)
    {
      fprintf(stderr,"failed to convert %s to cpt\n",name);
      return 1;
    }

  if (grad_save_gradient(ggr,file) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  grad_free_gradient(ggr);

  return 0;
}

static int svgpov_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "inc", svgpov_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  pov_t *pov;

  if (! svgpov_valid(svg, opt->verbose, opt->permissive))
    {
      fprintf(stderr,"cannot create valid povray file \n");
      return 1;
    }

  if ((pov = pov_new()) == NULL)
    {
      fprintf(stderr,"failed to create pov structure\n");
      return 1;
    }

  if (svgpov(svg,pov) != 0)
    {
      fprintf(stderr,"failed to convert %s to pov\n",name);
      return 1;
    }

  if (pov_write(file, pov) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  pov_destroy(pov);

  return 0;
}

static int svgsao_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "sao", svgsao_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  sao_t *sao;
  
  if ((sao = sao_new()) == NULL)
    {
      fprintf(stderr,"failed to create sao structure\n");
      return 1;
    }
  
  if (svgsao(svg,sao) != 0)
    {
      fprintf(stderr,"failed to convert %s to psp\n",opt->name);
      return 1;
    }
  
  if (sao_write(file, sao, name) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }
  
  sao_destroy(sao);

  return 0;
}


static int svggpt_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "gpt", svggpt_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  gpt_t *gpt;

  if ((gpt = gpt_new()) == NULL)
    {
      fprintf(stderr,"failed to create gpt structure\n");
      return 1;
    }

  if (svggpt(svg,gpt) != 0)
    {
      fprintf(stderr,"failed to convert %s to gpt\n",name);
      return 1;
    }

  if (gpt_write(file,gpt) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  gpt_destroy(gpt);

  return 0;
}

static int svgcss3_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "c3g", svgcss3_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  css3_t *css3;

  if ((css3 = css3_new()) == NULL)
    {
      fprintf(stderr,"failed to create css3 structure\n");
      return 1;
    }

  if (svgcss3(svg,css3) != 0)
    {
      fprintf(stderr,"failed to convert %s to css3\n",name);
      return 1;
    }

  if (css3_write(file,css3) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  css3_destroy(css3);

  return 0;
}

static int svgpsp_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "grd", svgpsp_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  psp_t *psp;

  if ((psp = psp_new()) == NULL)
    {
      fprintf(stderr,"failed to create psp structure\n");
      return 1;
    }
      
  if (svgpsp(svg, psp) != 0)
    {
      fprintf(stderr,"failed to convert %s to psp\n", name);
      return 1;
    }
            
  if (psp_write(file, psp) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  psp_destroy(psp);

  return 0;
}

static int svgpng_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "png", svgpng_dump);

  const char *name = (char*)svg->name;
  const char *file = opt->output.file;
  png_t *png;

  if ((png = png_new(opt->format.png.width, 
		     opt->format.png.height)) == NULL)
    {
      fprintf(stderr,"failed to create png structure\n");
      return 1;
    }
      
  if (svgpng(svg, png) != 0)
    {
      fprintf(stderr,"failed to convert %s to png\n", name);
      return 1;
    }
  
  if (png_write(file, png, name) != 0)
    {
      fprintf(stderr,"failed to write to %s\n", file);
      return 1;
    }

  png_destroy(png);

  return 0;
}

static int svgsvg_dump(const svg_t *svg, svgx_opt_t *opt)
{
  if (opt->job == job_all)
    return call_autonamed(svg, opt, "svg", svgsvg_dump);

  const char *file = opt->output.file;
  
  if (svg_write(file, svg, &(opt->format.svg.preview)) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  return 0;
}

/* check functions */

static int svgpov_valid(const svg_t *svg, int permissive, int verbose)
{
  int m = svg_num_stops(svg);
  
  if (m > POV_STOPS_MAX)
    {
      if (permissive)
	{
	  if (verbose)
	    printf("warning : format limit broken %i stops (max is %i)\n",
		   m,POV_STOPS_MAX);
	}
      else
	{
	  fprintf(stderr,
		  "format limit : POV-ray allows no more than %i stops,\n",
		  POV_STOPS_MAX);
	  fprintf(stderr,
		  "but this gradient has %i (use -p to ignore format limits)\n",m);
	  return 0;
	}
    }
  
  if (m < 2)
    {
      fprintf(stderr,"sanity check : found %i stops, but at least 2 required\n",m);
      return 0;
    }
  
  return 1;
}

/* conversion function */

static int svgcpt(const svg_t *svg, cpt_t *cpt)
{
  svg_node_t *node,*next;

  node = svg->nodes;
  next = node->r; 

  /* segments */

  while (next)
    {
      double z1,z2;

      z1 = node->stop.value;
      z2 = next->stop.value;

      if (z1 < z2)
	{
	  rgb_t c1,c2;
	  cpt_seg_t *seg;

	  c1 = node->stop.colour;
	  c2 = next->stop.colour;

	  if ((seg = cpt_seg_new()) == NULL)
	    {
	      fprintf(stderr,"failed to create cpt segment\n");
	      return 1;
	    }

	  seg->lsmp.val = z1;
	  seg->rsmp.val = z2;

	  seg->lsmp.fill.type         = fill_colour;
	  seg->lsmp.fill.u.colour.rgb = c1;

	  seg->rsmp.fill.type         = fill_colour;
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

/* convert an svg_t to a gradient_t */

static int svgggr(const svg_t *svg,gradient_t *ggr)
{
  svg_node_t *node,*next;
  grad_segment_t *gseg,*prev=NULL; 
  int n=0;

  ggr->name = strdup((char*)svg->name);

  node = svg->nodes;
  next = node->r; 

  /* segments */

  while (next)
    {
      double z1,z2;

      z1 = node->stop.value;
      z2 = next->stop.value;

      if (z1 < z2)
	{
	  rgb_t c1,c2;
	  double o1,o2;
	  double lcol[3],rcol[3];

	  c1 = node->stop.colour;
	  c2 = next->stop.colour;

	  o1 = node->stop.opacity;
	  o2 = next->stop.opacity;

	  if ((gseg = seg_new_segment()) == NULL) return 1;

	  gseg->prev = prev;

	  if (prev)
	    prev->next = gseg;
	  else
	    ggr->segments = gseg;

	  gseg->left   = z1/100.0; 
	  gseg->middle = (z1+z2)/200.0; 
	  gseg->right  = z2/100.0; 

	  lcol[0] = lcol[1] = lcol[2] = 0.0;
	  rcol[0] = rcol[1] = rcol[2] = 0.0;

	  gseg->a0 = o1;
	  gseg->a1 = o2;

	  rgb_to_rgbD(c1, lcol);
	  rgb_to_rgbD(c2, rcol);

	  gseg->r0 = lcol[0];
	  gseg->g0 = lcol[1];
	  gseg->b0 = lcol[2];
	  
	  gseg->r1 = rcol[0];
	  gseg->g1 = rcol[1];
	  gseg->b1 = rcol[2];

	  gseg->type  = GRAD_LINEAR;
	  gseg->color = GRAD_RGB;

	  prev = gseg;

	  n++;
	}

      node = next;
      next = node->r;
    } 

  return 0;
}

static double clampd(double z, double min, double max)
{
  if (z < min) z = min;
  if (z > max) z = max;

  return z;
}

static int clampi(int z, int min, int max)
{
  if (z < min) z = min;
  if (z > max) z = max;

  return z;
}

static int svgpsp(const svg_t *svg, psp_t *psp)
{
  int m,n;
  svg_node_t *node;
  psp_rgbseg_t *pcseg;
  psp_opseg_t *poseg;

  /* count & allocate */

  m = svg_num_stops(svg);
  
  if (m < 2)
    {
      fprintf(stderr,"bad number of stops : %i\n",m);
      return 1;
    }

  pcseg = malloc(m*sizeof(psp_rgbseg_t));
  poseg = malloc(m*sizeof(psp_opseg_t));
  
  if (! (pcseg && poseg))
    {
      fprintf(stderr,"failed to allocate segments\n");
      return 1;
    }

  /* convert segments */

  for (n=0, node = svg->nodes ; node ; n++,node = node->r)
    {
      rgb_t rgb;
      double op,z;

      rgb = node->stop.colour;
      op  = node->stop.opacity;
      z   = node->stop.value;
      
      pcseg[n].z        = clampd(4096*z/100.0, 0, 4096);
      pcseg[n].midpoint = 50;
      pcseg[n].r        = clampi(rgb.red*257,   0, 65535);
      pcseg[n].g        = clampi(rgb.green*257, 0, 65535);;
      pcseg[n].b        = clampi(rgb.blue*257,  0, 65535);;
  
      poseg[n].z        = clampd(4096*z/100.0, 0, 4096);
      poseg[n].midpoint = 50;
      poseg[n].opacity  = clampi(op*256, 0, 255);
    }

  /* copy name */

  char buffer[SVG_NAME_LEN];

  if (utf8_to_x("LATIN1", svg->name, buffer, SVG_NAME_LEN) != 0)
    {
      fprintf(stderr,"failed to convert utf name to latin1\n");
      return 1;
    }

  psp->name = (unsigned char*)strdup(buffer);

  psp->rgb.n   = m;
  psp->rgb.seg = pcseg; 

  psp->op.n    = m;
  psp->op.seg  = poseg; 

  return 0;
}

static int svgpov(const svg_t *svg, pov_t *pov)
{
  int n,m,nmod;
  svg_node_t *node;

  /* count & allocate */

  m = svg_num_stops(svg);
  
  if (m < 2)
    {
      fprintf(stderr,"bad number of stops : %i\n",m);
      return 1;
    }

  if (pov_stops_alloc(pov,m) != 0)
    {
      fprintf(stderr,"failed alloc for %i stops\n",m);
      return 1;
    }

  /* convert */

  for (n=0, node = svg->nodes ; node ; n++,node = node->r)
    {
      pov_stop_t stop;
      rgb_t rgb;
      double c[3],t,z;

      rgb = node->stop.colour;

      if (rgb_to_rgbD(rgb,c) != 0)
	{
	  fprintf(stderr,"failed conversion to rgbD\n");
	  return 1;
	}
      
      t = 1.0 - node->stop.opacity;
      
      if ((t < 0.0) || (t > 1.0))
	{
	  fprintf(stderr,"bad value for transparency : %f\n",t);
	  return 1;
	}
      
      z = node->stop.value/100.0;

      if ((z < 0.0) || (z > 1.0))
	{
	  fprintf(stderr,"bad z value : %f\n",t);
	  return 1;
	}
      
      stop.z = z;

      stop.rgbt[0] = c[0];
      stop.rgbt[1] = c[1];
      stop.rgbt[2] = c[2];
      stop.rgbt[3] = t;

      pov->stop[n] = stop;
    }

  if (n != m)
    {
      fprintf(stderr,
	      "missmatch between stops expected (%i) and found (%i)\n",
	      m,n);
      return 1;
    }

  pov->n = n;

  /* povray names need to be alphanumeric ascii */

  if (svg->name)
    {
      char aname[SVG_NAME_LEN];

      if (utf8_to_x("ASCII", svg->name, aname, SVG_NAME_LEN) != 0)
	{
	  printf("failed to convert name %s to ascii\n",aname);
	  return 1;
	}

      if (pov_set_name(pov,aname,&nmod) != 0)
	{
	  fprintf(stderr,
		  "failed to assign povray name (%s)\n",
		  aname);
	  return 1;
	}
 
      /* warn if name was modified */

      if (nmod > 0)
	fprintf(stderr,
		"name modified : %s to %s\n",
		svg->name,
		pov->name); 
    }
  else
    {      
      const char aname[] = "unassigned";

      if (pov_set_name(pov,aname,&nmod) != 0)
	{
	  fprintf(stderr,
		  "failed to assign povray name (%s)\n",
		  aname);
	  return 1;
	}
    }

  return 0;
}

static int svggpt(const svg_t *svg, gpt_t *gpt)
{
  int n,m;
  svg_node_t *node;

  /* count & allocate */

  m = svg_num_stops(svg);
  
  if (m < 2)
    {
      fprintf(stderr,"bad number of stops : %i\n",m);
      return 1;
    }

  if (gpt_stops_alloc(gpt,m) != 0)
    {
      fprintf(stderr,"failed alloc for %i stops\n",m);
      return 1;
    }

  /* convert */

  for (n=0, node = svg->nodes ; node ; n++,node = node->r)
    {
      gpt_stop_t stop;
      rgb_t rgb;
      double c[3];

      rgb = node->stop.colour;

      if (rgb_to_rgbD(rgb,c) != 0)
	{
	  fprintf(stderr,"failed conversion to rgbD\n");
	  return 1;
	}
      
      stop.z = node->stop.value/100.0;

      stop.rgb[0] = c[0];
      stop.rgb[1] = c[1];
      stop.rgb[2] = c[2];

      gpt->stop[n] = stop;
    }

  if (n != m)
    {
      fprintf(stderr,
	      "missmatch between stops expected (%i) and found (%i)\n",
	      m,n);
      return 1;
    }

  gpt->n = n;

  return 0;
}

static int svgsao(const svg_t *svg, sao_t *sao)
{
  svg_node_t *node;
  int n=0;

  for (node = svg->nodes ; node ; node = node->r, n++)
    {
      double rgbD[3], val = node->stop.value / 100.0;

      if (rgb_to_rgbD(node->stop.colour, rgbD) != 0)
	{
	  fprintf(stderr,"error converting colour of stop %i\n",n+1);
	  return 1;
	}

      int err = 0;

      err += sao_red_push(sao,val,rgbD[0]);
      err += sao_green_push(sao,val,rgbD[1]);
      err += sao_blue_push(sao,val,rgbD[2]);

      if (err)
	{
	  fprintf(stderr,"error adding sao stop %i\n",n+1);
	  return 1;
	}
    }
  
  return 0;
}

static int svgcss3(const svg_t *svg, css3_t *css3)
{
  int n,m;
  svg_node_t *node;

  /* count & allocate */

  m = svg_num_stops(svg);
  
  if (m < 2)
    {
      fprintf(stderr,"bad number of stops : %i\n",m);
      return 1;
    }

  if (css3_stops_alloc(css3,m) != 0)
    {
      fprintf(stderr,"failed alloc for %i stops\n",m);
      return 1;
    }

  /* convert */

  for (n=0, node = svg->nodes ; node ; n++,node = node->r)
    {
      css3_stop_t stop;

      stop.rgb   = node->stop.colour;      
      stop.z     = node->stop.value;
      stop.alpha = node->stop.opacity;

      css3->stop[n] = stop;
    }

  if (n != m)
    {
      fprintf(stderr,
	      "missmatch between stops expected (%i) and found (%i)\n",
	      m,n);
      return 1;
    }

  css3->n = n;

  return 0;
}

static int svgpng(const svg_t *svg, png_t *png)
{

  size_t nz = png->width;
  unsigned char *row = png->row;
  int i;

  for (i=0 ; i<nz ; i++)
    {
      rgb_t rgb;
      double op, z = 100.0 * (double)i/(double)(nz-1);

      if (svg_interpolate(svg, z, &rgb, &op) != 0)
	{
	  fprintf(stderr,"failed svg interpolate at %.3g\n", z);
	  return 1;
	}
      
      op *= 256;

      if (op > 255) op = 255;

      row[4*i]   = rgb.red;
      row[4*i+1] = rgb.green;
      row[4*i+2] = rgb.blue;
      row[4*i+3] = (unsigned char)op;
    }

  return 0;
}
