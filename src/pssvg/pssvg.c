/*
  pssvg.c
  (c) J.J.Green 2014
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <search.h>
#include <errno.h>

#include <grd5read.h>
#include <grd5.h>
#include <svgwrite.h>
#include <svg.h>
#include <gstack.h>
#include <grdxsvg.h>
#include <btrace.h>

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
	    gstack_pop(stack, &stop);
	}
    }

  while (! gstack_empty(stack0))
    {
      gstack_pop(stack0, &stop);
      gstack_push(stack, &stop);
    }

  gstack_destroy(stack0);

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

  gstack_destroy(stack0);

  return 0;
}

/*
  map a value in [0,1] to a double which is an integer 
  in 0 .. 255
*/

static double clamp_channel(double dval)
{
  int ival = floor(dval * 256);

  if (ival > 255) 
    ival = 255;
  else if (ival < 0)
    ival = 0;
  
  return ival;
}

/*
  The following functions modify their argument, converting
  them to RGB colour space

  These do not, in general, give the same values as PS since
  we are doing a naive and ham-fisted conversion not taking 
  into account colour profiles as PS does.  We could probably 
  do better if we used the open-source and widely available 
  Little CMS library (http://www.littlecms.com/) ... 
*/

static int grsc_to_rgb(grd5_colour_stop_t *stop)
{
  double val = stop->u.grsc.Gry;

  stop->u.rgb.Rd  = val;
  stop->u.rgb.Grn = val;
  stop->u.rgb.Bl  = val;

  stop->type = GRD5_MODEL_RGB;

  return 0;
}

static int hsb_to_rgb(grd5_colour_stop_t *stop)
{
  /* use the GIMP hsvD_to_rgbD (colour.h) function */

  double hsv[3];

  hsv[0] = stop->u.hsb.H / 360.0;
  hsv[1] = stop->u.hsb.Strt / 100.0;
  hsv[2] = stop->u.hsb.Brgh / 100.0;
  
  double rgb[3];

  hsvD_to_rgbD(hsv, rgb);
  
  stop->u.rgb.Rd  = clamp_channel(rgb[0]);
  stop->u.rgb.Grn = clamp_channel(rgb[1]);
  stop->u.rgb.Bl  = clamp_channel(rgb[2]);
	  
  stop->type = GRD5_MODEL_RGB;	  

  return 0;
}

static int cmyc_to_rgb(grd5_colour_stop_t *stop)
{
  /* Naive implementation */

  double 
    c = stop->u.cmyc.Cyn  / 100.0,
    m = stop->u.cmyc.Mgnt / 100.0,
    y = stop->u.cmyc.Ylw  / 100.0,
    k = stop->u.cmyc.Blck / 100.0;

  stop->u.rgb.Rd  = clamp_channel((1-c) * (1-k));
  stop->u.rgb.Grn = clamp_channel((1-m) * (1-k));
  stop->u.rgb.Bl  = clamp_channel((1-y) * (1-k));

  stop->type = GRD5_MODEL_RGB;

  return 0;
}

static double lab_xyz_curve(double W)
{
  double W3 = pow(W, 3);

  if (W3 > 0.008856)
    return W3;

  return (W - 16.0/116.0) / 7.787;
}

static double xyz_rgb_curve(double W)
{
  if (W > 0.0031308) 
    return 1.055 * pow(W, 1/2.4) - 0.055;

  return 12.92 * W;
}

#define REF_X  95.047
#define REF_Y 100.000
#define REF_Z 108.883

static int lab_to_rgb(grd5_colour_stop_t *stop)
{
  /* from easyrgb.com : CIE-L*ab -> XYZ */

  double
    L = stop->u.lab.Lmnc,
    A = stop->u.lab.A,
    B = stop->u.lab.B;
    
  double
    vY = (L+16.0) / 116.0,
    vX = A / 500.0 + vY,
    vZ = vY - B / 200.0;

  vX = lab_xyz_curve(vX);
  vY = lab_xyz_curve(vY);
  vZ = lab_xyz_curve(vZ);

  double
    X = REF_X * vX,
    Y = REF_Y * vY,
    Z = REF_Z * vZ;
    
  /* from easyrgb.com : XYZ -> RGB */

  vX = X / 100.0;
  vY = Y / 100.0;
  vZ = Z / 100.0;

  double
    vR =  3.2406 * vX - 1.5372 * vY - 0.4986 * vZ,
    vG = -0.9689 * vX + 1.8758 * vY + 0.0415 * vZ,
    vB =  0.0557 * vX - 0.2040 * vY + 1.0570 * vZ;

  vR = xyz_rgb_curve(vR);
  vG = xyz_rgb_curve(vG);
  vB = xyz_rgb_curve(vB);

  stop->u.rgb.Rd  = clamp_channel(vR);
  stop->u.rgb.Grn = clamp_channel(vG);
  stop->u.rgb.Bl  = clamp_channel(vB);

  stop->type = GRD5_MODEL_RGB;

  return 0;
}

/*
  convert the svg stops to the intermediate types, and rectify:
  replace the midpoints by explicit mid-point stops
*/

static gstack_t* rectify_rgb(grd5_grad_custom_t* gradc, pssvg_opt_t opt)
{
  grd5_colour_stop_t *grd5_stop = gradc->colour.stops;
  int i, n = gradc->colour.n;

  if (n<2)
    {
      btrace_add("input (grd5) has %i rgb stop(s)", n);
      return NULL;
    }

  for (i=0 ; i<n ; i++)
    {
      grd5_colour_stop_t *stop = grd5_stop + i;

      switch(stop->type)
	{
	case GRD5_MODEL_RGB:
	  break;

	case GRD5_MODEL_GRSC:
	  grsc_to_rgb(stop);
	  break;

	case GRD5_MODEL_HSB:
	  hsb_to_rgb(stop);
	  break;

	case GRD5_MODEL_CMYC:
	  cmyc_to_rgb(stop);
	  break;

	case GRD5_MODEL_LAB:
	  lab_to_rgb(stop);
	  break;

	case GRD5_MODEL_BCKC:
	  stop->u.rgb.Rd  = opt.bg.red;
	  stop->u.rgb.Grn = opt.bg.green;
	  stop->u.rgb.Bl  = opt.bg.blue;
	  stop->type = GRD5_MODEL_RGB;
	  break;

	case GRD5_MODEL_FRGC:
	  stop->u.rgb.Rd  = opt.fg.red;
	  stop->u.rgb.Grn = opt.fg.green;
	  stop->u.rgb.Bl  = opt.fg.blue;
	  stop->type = GRD5_MODEL_RGB;
	  break;

	case GRD5_MODEL_BOOK:
	  btrace_add("stop %i (book colour) not converted", i);
	  return NULL;

	default:
	  btrace_add("stop %i unknown colour type %i", i, stop->type);
	  return NULL;
	}

      if (stop->type != GRD5_MODEL_RGB)
	{
	  btrace_add("stop %i is non-RGB (type %i)", i, stop->type);
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

static gstack_t* rectify_op(grd5_grad_custom_t* gradc)
{
  grd5_transp_stop_t *grd5_stop = gradc->transp.stops;
  int i,n = gradc->transp.n;

  if (n<2)
    {
      btrace_add("input (grd5) has %i opacity stop(s)", n);
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

static int pssvg_title(grd5_grad_t *grd5_grad, 
		       svg_t *svg, 
		       pssvg_opt_t opt, 
		       int gradnum)
{
  size_t len;

  if (opt.title)
    len = snprintf((char*)(svg->name), SVG_NAME_LEN, opt.title, gradnum);
  else
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
	  btrace_add("failed ucs2 to utf8 conversion");
	  return 1;		   
	}

      /*
	prepare an initial entry for the hsearch() hash,
	we use the data field (which is a void*) as an 
	integer which is the count of title occurrences
      */

      ENTRY e, *ep;

      e.key  = strdup((char*)utf8_title);
      e.data = (void*)0;

      if ((ep = hsearch(e, ENTER)) == NULL)
	{
	  btrace_add("failed insert of %s into count hash", utf8_title);
	  btrace_add("hsearch error: %s", strerror(errno));
	  return 1;
	}

      uintptr_t title_count = (uintptr_t)(ep->data) + 1;
      ep->data = (void*)title_count;

      if (title_count > 1)
	{
	  /*
	    in this case the entry e was already in the hash 
	    table and so not inserted, hence we can free the 
	    storage allocated for the putative key
	  */

	  free(e.key);
	  len = snprintf((char*)svg->name, SVG_NAME_LEN, 
			 "%s_%lu", (char*)utf8_title, title_count);
	}
      else
	{
	  len = snprintf((char*)svg->name, SVG_NAME_LEN,
			 "%s", (char*)utf8_title);
	}
    }

  if (len >= SVG_NAME_LEN)
    {
      btrace_add("title truncated");
      return 1;
    }

  return 0;
}

static int pssvg_convert_one(grd5_grad_custom_t *grd5_gradc, 
			     svg_t *svg, 
			     pssvg_opt_t opt)
{
  gstack_t *rgbrec;
  int err = 0;

  if ((rgbrec = rectify_rgb(grd5_gradc, opt)) == NULL)
    err++;
  else
    {
      gstack_t *oprec;

      if ((oprec = rectify_op(grd5_gradc)) == NULL)
	err++;
      else
	{
	  err += (grdxsvg(rgbrec, oprec, svg) != 0);
	  gstack_destroy(oprec);
	}
      
      gstack_destroy(rgbrec);
    }

  if (err)
    btrace_add("failed conversion of rectified stops to svg");
  else if (opt.verbose)
    printf("  '%s', %i%% smooth; %i colour, %i opacity converted to %i RGBA\n", 
	   svg->name,
	   (int)round(grd5_gradc->interp/40.96),
	   grd5_gradc->colour.n,
	   grd5_gradc->transp.n,
	   svg_num_stops(svg));

  return err;
}

static int pssvg_convert_all(grd5_t *grd5, 
			     svgset_t *svgset, 
			     gstack_t *gstack, 
			     pssvg_opt_t opt)
{
  int i, n = grd5->n;

  if (opt.title == NULL)
    {
      /*
	In the case that titles are not autogenerated then we
	need to fix mutiple titles in the grd input (which is
	common) so that the output SVG titles are unique.  We
	do this by appending _2, _3, ... to repeated titles.
	So we need to keep track of how many occurenced of
	each title we have seen so far.  The appropriate data 
	structure is an associative array or hashmap, but 
	we don't really want to add another dependancy to the
	package; so we use the limited features of the POSIX 
	hsearch(3).

	Note that we do not call hdestroy() since that calls
	free() on the keys on some Unixes (BSD for example)
	and we only have at worst a few kB of keys, it does
	not seem worth the while to set up the conditional 
	compilation.
      */

      if (hcreate((size_t)ceil(1.25*n)) == 0)
	{
	  btrace_add("error in hcreate: %s", strerror(errno));
	  return 1;
	}
    }

  for (i=0 ; i<n ; i++)
    {
      grd5_grad_t *grd5_grad = grd5->gradients + i;
      svg_t *svg;

      switch (grd5_grad->type)
	{
	case GRD5_GRAD_CUSTOM:

	  if ((svg = svg_new()) == NULL) return 1;
	  if (pssvg_title(grd5_grad, svg, opt, i+1) != 0) return 1;
	  if (pssvg_convert_one(&(grd5_grad->u.custom), svg, opt) == 0)
	    {
	      if (gstack_push(gstack, &svg) != 0)
		{
		  btrace_add("error pushing result to stack");
		  return 1;
		}
	    }
	  else
	    {
	      btrace_add("failed convert of gradient %i", i);
	      svg_destroy(svg);
	    }

	  break;

	case GRD5_GRAD_NOISE:
	  
	  btrace_add("no conversion of (noise) gradient %i", i);
	  break;

	default:

	  btrace_add("bad type (%i) for gradient %i", grd5_grad->type, i);
	}
    }

  return 0;
}

static int pssvg_convert(grd5_t *grd5, svgset_t *svgset, pssvg_opt_t opt)
{
  int i, n = grd5->n;
  gstack_t *gstack;

  if ((gstack = gstack_new(sizeof(svg_t*), n, 1)) == NULL) 
    return 1;

  int err = pssvg_convert_all(grd5, svgset, gstack, opt);

  if (! err)
    {
      int m = gstack_size(gstack);

      if (m == 0)
	{
	  btrace_add("no gradients converted");
	  err++;
	}
      else
	{
	  if (m < n)
	    btrace_add("only %d/%d gradient converted", m, n); 

	  if (gstack_reverse(gstack) != 0)
	    return 1;

	  svgset->n = m;
	  
	  if ((svgset->svg = malloc(m*sizeof(svg_t*))) == NULL)
	    return 1;

	  for (i=0 ; i<m ; i++)
	    gstack_pop(gstack, svgset->svg+i); 
	}
    }

  gstack_destroy(gstack);

  return err;
}

extern int pssvg(pssvg_opt_t opt)
{
  int err = 0;
  grd5_t *grd5;

  switch (grd5_read(opt.file.input, &grd5))
    {
    case GRD5_READ_OK: 
      break;
    case GRD5_READ_FOPEN:
      btrace_add("failed to read %s", 
	      (opt.file.input ? opt.file.input : "stdin"));
      return 1;
    case GRD5_READ_FREAD:
      btrace_add("failed read from stream");
      return 1;
    case GRD5_READ_PARSE:
      btrace_add("failed to parse input");
      return 1;
    case GRD5_READ_NOT_GRD:
      btrace_add("not a GRD file");
      return 1;
    case GRD5_READ_NOT_GRD5:
      btrace_add("not a PhotoShop GRD file");
      return 1;
    case GRD5_READ_MALLOC:
      btrace_add("out of memory");
      return 1;
    case GRD5_READ_BUG:
      /* fall-through */
    default:
      btrace_add("internal error - please report this");
      return 1;
    }

  if (grd5->n == 0)
    {
      btrace_add("no gradients parsed");
      err++;
      goto cleanup_grd5;
    }

  if (opt.verbose)
    printf("parsed %i grd5 gradient%s\n", 
	   grd5->n,
	   (grd5->n == 1) ? "" : "s");

  svgset_t *svgset;

  if ((svgset = svgset_new()) == NULL)
    goto cleanup_grd5;

  if (pssvg_convert(grd5, svgset, opt) != 0)
    {
      btrace_add("conversion failed");
      err++;
      goto cleanup_svgset;
    }

  if (svgset->n == 0)
    {
      btrace_add("no svg gradients converted");
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
      btrace_add("failed write of svg");
      err++;
      goto cleanup_svgset;
    }

 cleanup_svgset:
  svgset_destroy(svgset);

 cleanup_grd5:
  grd5_destroy(grd5);

  return err;
}
