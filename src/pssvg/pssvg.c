/*
  pssvg.c
  (c) J.J.Green 2014
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <grd5read.h>
#include <grd5.h>
#include <svgwrite.h>
#include <svg.h>
#include <gstack.h>
#include <grdxsvg.h>

#include "ucs2utf8.h"
#include "pssvg.h"

typedef struct
{
  size_t n;
  svg_t **svg;
} svgset_t;

static svgset_t* svgset_new(void)
{
  svgset_t *svgset = malloc(sizeof(svgset_t));
  if (svgset == NULL)
    return NULL;

  svgset->n   = 0;
  svgset->svg = NULL;

  return svgset;
}

static void svgset_destroy(svgset_t *svgset)
{
  int i;

  for (i=0 ; i < svgset->n ; i++)
    svg_destroy(svgset->svg[i]);
  
  free(svgset->svg);
  free(svgset);
}

/* convert psp to intermediate types */

static double grd5_rgb_it(double x)
{
  return round(x)/256.0;
}

static double grd5_op_it(uint32_t x)
{
  return x/100.0;
}

static unsigned int grd5_z_it(uint32_t z)
{
  return (unsigned int)z*100;
}

static unsigned int grd5_zmid_it(uint32_t z0, 
				 uint32_t z1, 
				 uint32_t M)
{
  return (unsigned int)z0*100 + ((unsigned int)z1 - (unsigned int)z0)*M;
}


/* trim off excessive stops */

static int trim_rgb(gstack_t* stack)
{
  int n = gstack_size(stack);
  rgb_stop_t stop;
  gstack_t *stack0;

  if ((stack0 = gstack_new(sizeof(rgb_stop_t), n, n)) == NULL)
    return 1;

  while (! gstack_empty(stack))
    {
      gstack_pop(stack, &stop);
      gstack_push(stack0, &stop);

      if (stop.z >= 409600)
	{
	  while (! gstack_empty(stack))
	    gstack_pop(stack,&stop);
	}
    }

  while (! gstack_empty(stack0))
    {
      gstack_pop(stack0,&stop);
      gstack_push(stack,&stop);
    }

  return 0;
}

static int trim_op(gstack_t* stack)
{
  int n = gstack_size(stack);
  op_stop_t stop;
  gstack_t *stack0;

  if ((stack0 = gstack_new(sizeof(op_stop_t), n, n)) == NULL)
    return 1;

  while (! gstack_empty(stack))
    {
      gstack_pop(stack, &stop);
      gstack_push(stack0, &stop);

      if (stop.z >= 409600)
	{
	  while (! gstack_empty(stack))
	    gstack_pop(stack,&stop);
	}
    }

  while (! gstack_empty(stack0))
    {
      gstack_pop(stack0,&stop);
      gstack_push(stack,&stop);
    }

  return 0;
}

/*
  convert the sp stops to the intermediate types, 
  and rectify -- replace the midpoints by explicit 
  mid-point stops
*/

static gstack_t* rectify_rgb(grd5_grad_t* grad)
{
  grd5_colour_stop_t *grd5_stop = grad->colour.stops;
  int i, n = grad->colour.n;

  if (n<2)
    {
      fprintf(stderr,"input (grd5) has %i rgb stop(s)\n", n);
      return NULL;
    }

  for (i=0 ; i<n ; i++)
    {
      /* FIXME - convert non-RGB stops */

      if (grd5_stop[i].type != GRD5_STOP_RGB)
	{
	  fprintf(stderr, "stop %i is non-RGB\n", i+1);
	  return NULL;
	}
    }

  gstack_t *stack;

  if ((stack = gstack_new(sizeof(rgb_stop_t), 2*n, n)) == NULL)
    return NULL;

  rgb_stop_t stop;

  if (grd5_stop[0].Lctn > 0)
    {
      stop.z  = 0;
      stop.r = grd5_rgb_it(grd5_stop[0].u.rgb.Rd);
      stop.g = grd5_rgb_it(grd5_stop[0].u.rgb.Grn);
      stop.b = grd5_rgb_it(grd5_stop[0].u.rgb.Bl);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  for (i=0 ; i<n-1 ; i++)
    {
      stop.z = grd5_z_it(grd5_stop[i].Lctn);
      stop.r = grd5_rgb_it(grd5_stop[i].u.rgb.Rd);
      stop.g = grd5_rgb_it(grd5_stop[i].u.rgb.Grn);
      stop.b = grd5_rgb_it(grd5_stop[i].u.rgb.Bl);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
      
      if (grd5_stop[i].Mdpn != 50)
	{
	  stop.z = grd5_zmid_it(grd5_stop[i].Lctn, 
				grd5_stop[i+1].Lctn, 
				grd5_stop[i].Mdpn);
	  stop.r = 0.5*(grd5_rgb_it(grd5_stop[i].u.rgb.Rd) +
			grd5_rgb_it(grd5_stop[i+1].u.rgb.Rd));
	  stop.g = 0.5*(grd5_rgb_it(grd5_stop[i].u.rgb.Grn) + 
			grd5_rgb_it(grd5_stop[i+1].u.rgb.Grn));
	  stop.b = 0.5*(grd5_rgb_it(grd5_stop[i].u.rgb.Bl) + 
			grd5_rgb_it(grd5_stop[i+1].u.rgb.Bl));

	  if (gstack_push(stack, &stop) != 0)
	    return NULL;
	}
    }

  stop.z = grd5_z_it(grd5_stop[n-1].Lctn);
  stop.r = grd5_rgb_it(grd5_stop[n-1].u.rgb.Rd);
  stop.g = grd5_rgb_it(grd5_stop[n-1].u.rgb.Grn);
  stop.b = grd5_rgb_it(grd5_stop[n-1].u.rgb.Bl);

  if (gstack_push(stack, &stop) != 0)
    return NULL;

  /* add implicit final stop */

  if (stop.z < 409600)
    {
      stop.z = 409600;

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  if (gstack_reverse(stack) != 0)
    return NULL;

  if (trim_rgb(stack) != 0)
    return NULL;

  return stack;
}

static gstack_t* rectify_op(grd5_grad_t* grad)
{
  grd5_transp_stop_t *grd5_stop = grad->transp.stops;
  int i,n = grad->transp.n;

  if (n<2)
    {
      fprintf(stderr,"input (grd5) has %i opacity stop(s)\n",n);
      return NULL;
    }

  gstack_t *stack;

  if ((stack = gstack_new(sizeof(op_stop_t), 2*n, n)) == NULL)
    return NULL;

  op_stop_t stop;

  if (grd5_stop[0].Lctn > 0)
    {
      stop.z  = 0;
      stop.op = grd5_op_it(grd5_stop[0].Opct);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  for (i=0 ; i<n-1 ; i++)
    {
      stop.z  = grd5_z_it(grd5_stop[i].Lctn);
      stop.op = grd5_op_it(grd5_stop[i].Opct);

      if (gstack_push(stack, &stop) != 0)
	return NULL;
      
      if (grd5_stop[i].Mdpn != 50)
	{
	  stop.z  = grd5_zmid_it(grd5_stop[i].Lctn, 
				 grd5_stop[i+1].Lctn, 
				 grd5_stop[i].Mdpn);
	  stop.op = 0.5*(grd5_op_it(grd5_stop[i].Opct) + 
			 grd5_op_it(grd5_stop[i+1].Opct));

	  if (gstack_push(stack, &stop) != 0)
	    return NULL;
	}
    }

  stop.z  = grd5_z_it(grd5_stop[n-1].Lctn);
  stop.op = grd5_op_it(grd5_stop[n-1].Opct);

  if (gstack_push(stack, &stop) != 0)
    return NULL;

  if (stop.z < 409600)
    {
      stop.z = 409600;
      if (gstack_push(stack, &stop) != 0)
	return NULL;
    }

  if (gstack_reverse(stack) != 0)
    return NULL;

  if (trim_op(stack) != 0)
    return NULL;

  return stack;
}

static int pssvg_convert1(grd5_grad_t *grd5_grad, 
			  svg_t **psvg, 
			  pssvg_opt_t opt)
{
  grd5_string_t *ucs2_title_string = grd5_grad->title;

  size_t ucs2_title_len = ucs2_title_string->len;
  char  *ucs2_title     = ucs2_title_string->content;
  size_t utf8_title_len = ucs2_title_len; 
  char   utf8_title[ucs2_title_len];

  if (ucs2_to_utf8(ucs2_title,
		   ucs2_title_len,
		   utf8_title,
		   utf8_title_len) != 0)
    {
      fprintf(stderr,"failed ucs2 to utf8 conversion\n");
      return 1;		   
    }

  svg_t *svg;

  if ((svg = svg_new()) == NULL) return 1;
  strncpy((char*)svg->name, (char*)utf8_title, SVG_NAME_LEN);

  gstack_t *rgbrec, *oprec;

  if ((rgbrec = rectify_rgb(grd5_grad)) == NULL)
    return 1;
  
  if ((oprec = rectify_op(grd5_grad)) == NULL)
    return 1;

  if (grdxsvg(rgbrec, oprec, svg) != 0)
    {
      fprintf(stderr, "failed conversion of rectified stops to svg\n");
      return 1;
    }

  if (opt.verbose)
    {
      printf("'%s' : %i RGB, %i Opacity converted to %i RGBA\n", 
	     utf8_title,
	     grd5_grad->colour.n,
	     grd5_grad->transp.n,
	     svg_num_stops(svg));
    }

  *psvg = svg;

  return 0;
}

static int pssvg_convert(grd5_t *grd5, svgset_t *svgset, pssvg_opt_t opt)
{
  int i, n = grd5->n;

  gstack_t *gstack;

  if ((gstack = gstack_new(sizeof(svg_t*), n, 1)) == NULL) 
    return 1;

  for (i=0 ; i<n ; i++)
    {
      grd5_grad_t *grd5_grad = grd5->gradients + i;
      svg_t *svg;

      if (pssvg_convert1(grd5_grad, &svg, opt) == 0)
	{
	  if (gstack_push(gstack, &svg) != 0)
	    {
	      fprintf(stderr, "error pusing result to stack\n");
	      return 1;
	    }
	}
      else
	{
	  fprintf(stderr, "failed convert of gradient '%i'\n", i);
	}
    }

  int m = gstack_size(gstack);

  if (m == 0)
    {
      fprintf(stderr, "no gradients converted\n");
      return 1;
    }

  if (m < n)
    fprintf(stderr, "only %d/%d gradient converted\n", m, n); 

  svgset->n   = m;
  svgset->svg = malloc(m*sizeof(svg_t*));

  for (i=0 ; i<m ; i++)
    gstack_pop(gstack, svgset->svg+i); 

  return 0;
}

extern int pssvg(pssvg_opt_t opt)
{
  grd5_t *grd5;

  switch (grd5_read(opt.file.input, &grd5))
    {
    case GRD5_READ_OK: 
      break;
    case GRD5_READ_FOPEN:
      fprintf(stderr, "failed to read %s\n", 
	      (opt.file.input ? opt.file.input : "stdin"));
      return 1;
    case GRD5_READ_FREAD:
      fprintf(stderr, "failed read from stream\n");
      return 1;
    case GRD5_READ_PARSE:
      fprintf(stderr, "failed to parse input\n");
      return 1;
    case GRD5_READ_MALLOC:
      fprintf(stderr, "out of memory\n");
      return 1;
    case GRD5_READ_BUG:
      /* fall-through */
    default:
      fprintf(stderr, "internal error - please report this\n");
      return 1;
    }

  int err = 0;
  svgset_t *svgset;

  if ((svgset = svgset_new()) == NULL)
    goto cleanup_grd5;

  if (pssvg_convert(grd5, svgset, opt) != 0)
    {
      fprintf(stderr, "conversion failed\n");
      err++;
      goto cleanup_svgset;
    }

  if (svgset->n == 0)
    {
      fprintf(stderr, "no svg gradients converted\n");
      err++;
      goto cleanup_svgset;
    }

  svg_preview_t preview;
  preview.use = false;

  if (svg_write(opt.file.output, 
		svgset->n, 
		(const svg_t**)svgset->svg, 
		&preview) != 0)
    {
      fprintf(stderr, "failed write of svg\n");
      err++;
      goto cleanup_svgset;
    }

 cleanup_svgset:
  svgset_destroy(svgset);

 cleanup_grd5:
  grd5_destroy(grd5);

  return err;
}
