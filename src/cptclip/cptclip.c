/*
  cptclip.c

  (c) J.J.Green 2010
  $Id: cptclip.c,v 1.3 2010/04/12 21:36:52 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cptio.h"

#include "cptclip.h"

static int cptclip_convert(cpt_t*,cptclip_opt_t);

extern int cptclip(char* infile,char* outfile,cptclip_opt_t opt)
{
  cpt_t *cpt;
  int err = 0;

  if ((cpt = cpt_new()) != NULL)
    {
      if (cpt_read(infile,cpt,0) == 0)
	{
	  if (cptclip_convert(cpt,opt) == 0)
	    {
	      if (cpt_write(outfile,cpt) == 0)
		{
		  /* success */
		}
	      else
		{
		  fprintf(stderr,"error writing cpt struct\n");
		  err = 1;
		}
	    }
	  else
	    {
	      fprintf(stderr,"failed to convert\n");
	      err = 1;
	    }
	}
      else
	{
	  fprintf(stderr,"failed to load cpt from %s\n",
		  (infile ? infile : "<stdin>"));
	  err = 1;
	}
      
      cpt_destroy(cpt);    
    }	
  else
    {
      fprintf(stderr,"failed to create cpt struct\n");
      err = 1;
    }
  
  if (err)
    fprintf(stderr,"failed to write cpt to %s\n",
	    (outfile ? outfile : "<stdout>"));
  
  return err;
}

static int cptclip_z_inc(cpt_t*,cptclip_opt_t);
static int cptclip_z_dec(cpt_t*,cptclip_opt_t);

static int cptclip_convert(cpt_t* cpt,cptclip_opt_t opt)
{
  if (cpt->segment == NULL)
    {
      fprintf(stderr,"cpt has no segments\n");
      return 1;
    }

  if (opt.segments)
    {
      int i,nseg = cpt_nseg(cpt);

      for (i=1 ; i<opt.u.segs.min ; i++)
	cpt_pop(cpt);

      for (i=nseg ; i>opt.u.segs.max ; i--)
	cpt_shift(cpt);

      return 0;
    }

  return (
	  cpt_increasing(cpt) ? 
	  cptclip_z_inc(cpt,opt) :
	  cptclip_z_dec(cpt,opt)
	  );
}

static int cptclip_z_inc(cpt_t* cpt,cptclip_opt_t opt)
{
  cpt_seg_t* seg;

  /*
    pop segments from the left until one of them
    intersects the requested interval
  */

  while ((seg = cpt_pop(cpt)) != NULL)
    if (seg->rsmp.val > opt.u.z.min) break;

  if (seg)
    {
      /*
	if this segment is not completely inside the
	requested interval then clip it to the interval
      */

      if (seg->lsmp.val < opt.u.z.min)
	{
	  fill_t fill;
	  double 
	    len = seg->rsmp.val - seg->lsmp.val,
	    z   = (opt.u.z.min - seg->lsmp.val)/len; 

	  if  (fill_interpolate(z,
				seg->lsmp.fill,
				seg->rsmp.fill,
				cpt->model,
				&fill) != 0)
            {
              fprintf(stderr,"fill interpolation failed\n");
              return 1;
            }

	  seg->lsmp.fill = fill;
	  seg->lsmp.val  = opt.u.z.min;
	}

      /*
	push the (possibly modified) segment back to 
	where it came
      */

      cpt_prepend(seg,cpt);
    }

  /* likewise on the right */

  while ((seg = cpt_shift(cpt)) != NULL)
    if (seg->lsmp.val < opt.u.z.max) break;

  if (seg)
    {
      if (seg->rsmp.val > opt.u.z.max)
	{
	  fill_t fill;
	  double 
	    len = seg->rsmp.val - seg->lsmp.val,
	    z   = (opt.u.z.max - seg->lsmp.val)/len; 

	  if  (fill_interpolate(z,
				seg->lsmp.fill,
				seg->rsmp.fill,
				cpt->model,
				&fill) != 0)
            {
              fprintf(stderr,"fill interpolation failed\n");
              return 1;
            }

	  seg->rsmp.fill = fill;
	  seg->rsmp.val  = opt.u.z.max;
	}

      cpt_append(seg,cpt);
    }

  return 0;
}

static int cptclip_z_dec(cpt_t* cpt,cptclip_opt_t opt)
{
  // FIXME

  fprintf(stderr,"decreasing gradients not handled yet\n");
  return 1;
}
