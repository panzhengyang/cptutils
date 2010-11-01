/*
  svgx.c : convert svg file to cpt file
 
  $Id: svgx.c,v 1.18 2008/04/13 21:13:51 jjg Exp jjg $
  J.J. Green 2005

  TODO  
  - conversion of implicit first/last segments  
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "colour.h"
#include "svgread.h"

#include "cpt.h"
#include "cptio.h"
#include "gradient.h"
#include "povwrite.h"
#include "gptwrite.h"
#include "pspwrite.h"

#include "svgx.h"

static int svgx_list(svgx_opt_t,svg_list_t*);
static int svgx_named(svgx_opt_t,svg_list_t*);
static int svgx_all(svgx_opt_t,svg_list_t*);

static int svgcpt(svg_t*,cpt_t*);
static int svgggr(svg_t*,gradient_t*);
static int svgpsp(svg_t*,psp_t*);
static int svgpov(svg_t*,pov_t*);
static int svggpt(svg_t*,gpt_t*);

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

/*
  check limits for povray
*/

static int svgpov_valid(svg_t* svg,int permissive,int verbose)
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

/* convert a named gradient */

static int svg_select_name(svg_t*,char*);
static int svg_select_first(svg_t*,char*);

static int svgx_named(svgx_opt_t opt,svg_list_t* list)
{
  svg_t *svg;
  cpt_t *cpt;
  pov_t *pov;
  gpt_t *gpt;
  psp_t *psp;
  gradient_t *ggr;
  char *file;

  /* get svg with this name */

  if (opt.name)
    {
      svg = svg_list_select(list,(int (*)(svg_t*,void*))svg_select_name,opt.name);

      if (!svg)
	{
	  fprintf(stderr,"couldn't find gradient named %s\n",opt.name);
	  return 1;
	}
    }
  else if (opt.first)
    {
      svg = svg_list_select(list,(int (*)(svg_t*,void*))svg_select_first,NULL);

      if (!svg)
	{
	  fprintf(stderr,"couldn't find first gradient!\n");
	  return 1;
	}
    }
  else 
    return 0;

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

      /* housekeeping */

      cpt->model = rgb;

      cpt->fg.type = cpt->bg.type = cpt->nan.type = colour;
      
      cpt->bg.u.colour.rgb  = opt.bg;
      cpt->fg.u.colour.rgb  = opt.fg;
      cpt->nan.u.colour.rgb = opt.nan;
  
      if (snprintf(cpt->name,CPT_NAME_LEN,"%s",svg->name) >= CPT_NAME_LEN)
	{
	  fprintf(stderr,"cpt name truncated\n");
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

      if ((ggr = grad_new_gradient()) == NULL)
	{
	  fprintf(stderr,"failed to create ggr structure\n");
	  return 1;
	}
      
      if (svgggr(svg,ggr) != 0)
	{
	  fprintf(stderr,"failed to convert %s to ggr\n",opt.name);
	  return 1;
	}
            
      if (grad_save_gradient(ggr,file) != 0)
	{
	  fprintf(stderr,"failed to write to %s\n",(file ? file : "<stdout>"));
	  return 1;
	}
      
      grad_free_gradient(ggr);

      break;

    case type_psp:

      if ((psp = psp_new()) == NULL)
	{
	  fprintf(stderr,"failed to create psp structure\n");
	  return 1;
	}
      
      if (svgpsp(svg,psp) != 0)
	{
	  fprintf(stderr,"failed to convert %s to psp\n",opt.name);
	  return 1;
	}
            
      if (psp_write(file,psp) != 0)
	{
	  fprintf(stderr,"failed to write to %s\n",(file ? file : "<stdout>"));
	  return 1;
	}

      psp_destroy(psp);

      break;

    case type_pov:
      
      if (! svgpov_valid(svg, opt.permissive, opt.verbose))
	{
	  fprintf(stderr,"cannot create valid povray file\n");
	  return 1;
	}

      if ((pov = pov_new()) == NULL)
	{
	  fprintf(stderr,"failed to create pov structure\n");
	  return 1;
	}

      if (svgpov(svg,pov) != 0)
	{
	  fprintf(stderr,"failed to convert %s to pov\n",opt.name);
	  return 1;
	}
            
      if (pov_write(file,pov) != 0)
	{
	  fprintf(stderr,"failed to write to %s\n",(file ? file : "<stdout>"));
	  return 1;
	}

      pov_destroy(pov);

      break;

    case type_gpt:
      
      if ((gpt = gpt_new()) == NULL)
	{
	  fprintf(stderr,"failed to create gpt structure\n");
	  return 1;
	}

      if (svggpt(svg,gpt) != 0)
	{
	  fprintf(stderr,"failed to convert %s to gpt\n",opt.name);
	  return 1;
	}
            
      if (gpt_write(file,gpt) != 0)
	{
	  fprintf(stderr,"failed to write to %s\n",(file ? file : "<stdout>"));
	  return 1;
	}

      gpt_destroy(gpt);

      break;

    default:

      fprintf(stderr,"strange output format!\n");
      return 1;
    }

  if (opt.verbose)
    printf("wrote %s to %s\n",
	   (opt.name ? opt.name : "gradient"),
	   (file ? file : "<stdout>"));

  return 0;
}

static int svg_select_first(svg_t* svg,char* name)
{
  return 1;
}

static int svg_select_name(svg_t* svg,char* name)
{
  return (strcmp(svg->name,name) == 0 ? 1 : 0);
}

static int svgcpt_dump(svg_t*,svgx_opt_t*);
static int svgggr_dump(svg_t*,svgx_opt_t*);
static int svgpov_dump(svg_t*,svgx_opt_t*);
static int svggpt_dump(svg_t*,svgx_opt_t*);
static int svgpsp_dump(svg_t*,svgx_opt_t*);

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

      dump = (int (*)(svg_t*,void*))svgggr_dump;
      break;

    case type_pov:

      dump = (int (*)(svg_t*,void*))svgpov_dump;
      break;

    case type_gpt:

      dump = (int (*)(svg_t*,void*))svggpt_dump;
      break;

    case type_psp:

      dump = (int (*)(svg_t*,void*))svgpsp_dump;
      break;

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

  /* housekeeping */

  cpt->model = rgb;
  
  cpt->fg.type = cpt->bg.type = cpt->nan.type = colour;
  
  cpt->bg.u.colour.rgb  = opt->bg;
  cpt->fg.u.colour.rgb  = opt->fg;
  cpt->nan.u.colour.rgb = opt->nan;
  
  if (snprintf(cpt->name,CPT_NAME_LEN,"%s",name) >= CPT_NAME_LEN)
    {
      fprintf(stderr,"gradient name (%s) truncated\n",name);
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

static int svgggr_dump(svg_t* svg,svgx_opt_t* opt)
{
  int  n = SVG_NAME_LEN+5;
  char file[n],*name;
  gradient_t* ggr;

  if (!svg) return 1;

  name = svg->name;

  if (snprintf(file,n,"%s.ggr",name) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  if ((ggr = grad_new_gradient()) == NULL)
    {
      fprintf(stderr,"failed to create ggr structure\n");
      return 1;
    }

  /* translate */

  if (svgggr(svg,ggr) != 0)
    {
      fprintf(stderr,"failed to convert %s to cpt\n",name);
      return 1;
    }

  /* write */

  if (grad_save_gradient(ggr,file) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  grad_free_gradient(ggr);

  if (opt->verbose)  
    printf("  %s\n",file);

  return 0;
}

static int svgpov_dump(svg_t* svg,svgx_opt_t* opt)
{
  int  n = SVG_NAME_LEN+5;
  char file[n],*name;
  pov_t* pov;

  if (!svg) return 1;

  if (! svgpov_valid(svg, opt->verbose, opt->permissive))
    {
      fprintf(stderr,"cannot create valid povray filen");
      return 1;
    }

  name = svg->name;

  if (snprintf(file,n,"%s.inc",name) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  if ((pov = pov_new()) == NULL)
    {
      fprintf(stderr,"failed to create pov structure\n");
      return 1;
    }

  /* translate */

  if (svgpov(svg,pov) != 0)
    {
      fprintf(stderr,"failed to convert %s to pov\n",name);
      return 1;
    }

  /* write */

  if (pov_write(file,pov) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  pov_destroy(pov);

  if (opt->verbose)  
    printf("  %s\n",file);

  return 0;
}

static int svggpt_dump(svg_t* svg,svgx_opt_t* opt)
{
  int  n = SVG_NAME_LEN+5;
  char file[n],*name;
  gpt_t* gpt;

  if (!svg) return 1;

  name = svg->name;

  if (snprintf(file,n,"%s.gpt",name) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  if ((gpt = gpt_new()) == NULL)
    {
      fprintf(stderr,"failed to create gpt structure\n");
      return 1;
    }

  /* translate */

  if (svggpt(svg,gpt) != 0)
    {
      fprintf(stderr,"failed to convert %s to gpt\n",name);
      return 1;
    }

  /* write */

  if (gpt_write(file,gpt) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  gpt_destroy(gpt);

  if (opt->verbose)  
    printf("  %s\n",file);

  return 0;
}

static int svgpsp_dump(svg_t* svg,svgx_opt_t* opt)
{
  int  n = SVG_NAME_LEN+5;
  char file[n],*name;
  psp_t* psp;

  if (!svg) return 1;

  name = svg->name;

  if (snprintf(file,n,"%s.grd",name) >= n)
    {
      fprintf(stderr,"filename truncated! %s\n",file);
      return 1;
    }

  if ((psp = psp_new()) == NULL)
    {
      fprintf(stderr,"failed to create psp structure\n");
      return 1;
    }
      
  if (svgpsp(svg,psp) != 0)
    {
      fprintf(stderr,"failed to convert %s to psp\n",opt->name);
      return 1;
    }
            
  if (psp_write(file,psp) != 0)
    {
      fprintf(stderr,"failed to write to %s\n",file);
      return 1;
    }

  psp_destroy(psp);

  if (opt->verbose)  
    printf("  %s\n",file);

  return 0;
}

/* coonvert an svg_t to a cpt_t */

static int svgcpt(svg_t* svg,cpt_t* cpt)
{
  svg_node_t *node,*next;

  node = svg->nodes;
  next = node->r; 

  /* handle implicit first segment */

  if (node->stop.value > 0.0)
    {
      double z1,z2;
      rgb_t c;
      cpt_seg_t* seg;

      z1 = 0.0;
      z2 = node->stop.value;
      c  = node->stop.colour;

      if ((seg = cpt_seg_new()) == NULL)
	{
	  fprintf(stderr,"failed to create cpt segment\n");
	  return 1;
	}

      seg->lsmp.val = z1;
      seg->rsmp.val = z2;
      
      seg->lsmp.fill.type         = colour;
      seg->lsmp.fill.u.colour.rgb = c;

      seg->rsmp.fill.type         = colour;
      seg->rsmp.fill.u.colour.rgb = c;

      if (cpt_append(seg,cpt) != 0)
	{
	  fprintf(stderr,"failed to append segment\n");
	  return 1;
	}
    }

  /* middle segments */

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

  /* handle implicit final segment */

  if (node->stop.value < 100.0)
    {
      double z1,z2;
      rgb_t c;
      cpt_seg_t* seg;

      z1 = node->stop.value;
      z2 = 100.0;
      c  = node->stop.colour;

      if ((seg = cpt_seg_new()) == NULL)
	{
	  fprintf(stderr,"failed to create cpt segment\n");
	  return 1;
	}

      seg->lsmp.val = z1;
      seg->rsmp.val = z2;
      
      seg->lsmp.fill.type         = colour;
      seg->lsmp.fill.u.colour.rgb = c;

      seg->rsmp.fill.type         = colour;
      seg->rsmp.fill.u.colour.rgb = c;

      if (cpt_append(seg,cpt) != 0)
	{
	  fprintf(stderr,"failed to append segment\n");
	  return 1;
	}
    }

  return 0;
}

/* coonvert an svg_t to a gradient_t */

static int svgggr(svg_t* svg,gradient_t* ggr)
{
  svg_node_t *node,*next;
  grad_segment_t *gseg,*prev=NULL; 
  int n=0;

  ggr->name = strdup(svg->name);

  node = svg->nodes;
  next = node->r; 

  /* implicit first segment */

  if (node->stop.value > 0.0)
    {
      double z1,z2;
      rgb_t c;
      double o;
      double col[3];

      z1 = 0.0;
      z2 = node->stop.value;

      c = node->stop.colour;
      o = node->stop.opacity;

      if ((gseg = seg_new_segment()) == NULL) return 1;

      gseg->prev = prev;
      
      if (prev) prev->next = gseg;
      else ggr->segments = gseg;

      gseg->left   = z1/100.0; 
      gseg->middle = (z1+z2)/200.0;
      gseg->right  = z2/100.0; 

      col[0] = col[1] = col[2] = 0.0;

      gseg->a0 = o;
      gseg->a1 = o;

      rgb_to_rgbD(c,col);

      gseg->r0 = col[0];
      gseg->g0 = col[1];
      gseg->b0 = col[2];
	  
      gseg->r1 = col[0];
      gseg->g1 = col[1];
      gseg->b1 = col[2];

      gseg->type  = GRAD_LINEAR;
      gseg->color = GRAD_RGB;
      
      prev = gseg;

      n++;
    } 

  /* main segments */

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

  if (node->stop.value < 100.0)
    {
      double z1,z2;
      rgb_t c;
      double o;
      double col[3];

      z1 = node->stop.value;
      z2 = 100.0;

      c = node->stop.colour;
      o = node->stop.opacity;

      if ((gseg = seg_new_segment()) == NULL) return 1;

      gseg->prev = prev;
      
      if (prev) prev->next = gseg;
      else ggr->segments = gseg;

      gseg->left   = z1/100.0; 
      gseg->middle = (z1+z2)/200.0;
      gseg->right  = z2/100.0; 

      col[0] = col[1] = col[2] = 0.0;

      gseg->a0 = o;
      gseg->a1 = o;

      rgb_to_rgbD(c,col);

      gseg->r0 = col[0];
      gseg->g0 = col[1];
      gseg->b0 = col[2];
	  
      gseg->r1 = col[0];
      gseg->g1 = col[1];
      gseg->b1 = col[2];

      gseg->type  = GRAD_LINEAR;
      gseg->color = GRAD_RGB;
      
      prev = gseg;

      n++;
    } 

  return 0;
}

static int svgpsp(svg_t* svg,psp_t* psp)
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
      int op;
      double z;

      rgb = node->stop.colour;
      op  = node->stop.opacity;
      z   = node->stop.value;
      
      pcseg[n].z        = 4096*z/100.0;
      pcseg[n].midpoint = 50;
      pcseg[n].r        = rgb.red*257;
      pcseg[n].g        = rgb.green*257;
      pcseg[n].b        = rgb.blue*257;
  
      poseg[n].z        = 4096*z/100.0;
      poseg[n].midpoint = 50;
      poseg[n].opacity  = op*255;
    }

  /* copy across */

  psp->name = strdup(svg->name);

  psp->rgb.n   = m;
  psp->rgb.seg = pcseg; 

  psp->op.n    = m;
  psp->op.seg  = poseg; 

  return 0;
}

static int svgpov(svg_t* svg,pov_t* pov)
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

  if (pov_set_name(pov,(svg->name ? svg->name : "unnamed"),&nmod) != 0)
    {
      fprintf(stderr,
	      "failed to assign povray name (%s)\n",
	      (svg->name ? svg->name : "(null)"));
      return 1;
    }

  /* warn if name was modified */

  if (nmod > 0)
    fprintf(stderr,
	    "name modified : %s to %s\n",
	    svg->name, pov->name); 

  return 0;
}

static int svggpt(svg_t* svg, gpt_t* gpt)
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
