/*
  cptcont.c

  (c) J.J.Green 2010
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cptread.h>
#include <cptwrite.h>
#include <btrace.h>

#include "cptcont.h"

static int cptcont_convert(cpt_t*,cptcont_opt_t);

extern int cptcont(char* infile,char* outfile,cptcont_opt_t opt)
{
  cpt_t *cpt;
  int err=0;

  if ((cpt = cpt_new()) != NULL)
    {
      if (cpt_read(infile, cpt) == 0)
	{
	  if (cptcont_convert(cpt, opt) == 0)
	    {
	      if (cpt_write(outfile, cpt) == 0)
		{
		  /* success */
		}
	      else
		{
		  btrace("error writing cpt struct");
		  err = 1;
		}
	    }
	  else
	    {
	      btrace("failed to convert");
	      err = 1;
	    }
	}
      else
	{
	  btrace("failed to load cpt from %s",
		     (infile ? infile : "<stdin>"));
	  err = 1;
	}
      
      cpt_destroy(cpt);    
    }	
  else
    {
      btrace("failed to create cpt struct");
      err = 1;
    }
  
  if (err)
    btrace("failed to write cpt to %s",
	       (outfile ? outfile : "<stdout>"));
  
  return err;
}

static int midpoint_split(cpt_seg_t*,model_t);

static int cptcont_convert(cpt_t* cpt,cptcont_opt_t opt)
{
  /* check we have at least one cpt segment */

  if (cpt->segment == NULL)
    {
      btrace("cpt has no segments");
      return 1;
    }

  /* midpoint splitting */

  if (opt.midpoint)
    {
      if (opt.verbose)
	printf("splitting at midpoints\n");

      if (midpoint_split(cpt->segment,cpt->model) != 0)
	{
	  btrace("error splitting at midpoints");
	  return 1;
	}
    }

  /* convert cpt segments */

  cpt_seg_t *s1,*s2;
  int n = 0;
  double p = opt.partial/2;

  for (s1 = cpt->segment, s2 = s1->rseg ; 
       s2 ; 
       s1 = s2, s2 = s1->rseg)
    {
      if (s1->rsmp.fill.type != s2->lsmp.fill.type)
        {
          btrace("sorry, can't convert mixed fill types");
          return 1;
        }

      if (! fill_eq(s1->rsmp.fill,s2->lsmp.fill, cpt->model))
	{
	  fill_t F1,F2;
	  int err = 0;

	  err += fill_interpolate(p,
				  s1->rsmp.fill,
				  s2->lsmp.fill,
				  cpt->model,
				  &F1);
	  err += fill_interpolate(p,
				  s2->lsmp.fill,
				  s1->rsmp.fill,
				  cpt->model,
				  &F2);
	  
	  if (err)
	    {
	      btrace("failed fill intepolate");
	      return 1;
	    }

	  s1->rsmp.fill = F1;
	  s2->lsmp.fill = F2;

	  n++;
	}
    }

  if (opt.verbose)
    printf("modified %i discontinuities\n",n);
  
  return 0;
}

/*
  recursive function which splits seach segment into
  two -- the old segment on the left, the new one on
  the right
*/

static int midpoint_split(cpt_seg_t* s,model_t model)
{
  if (!s) return 0;

  int err; 

  if ((err = midpoint_split(s->rseg,model)) != 0) 
    return err + 1;

  double zm = (s->lsmp.val + s->rsmp.val)/2.0; 

  /* the new left and right segments */

  cpt_seg_t 
    *sl = s,
    *sr = cpt_seg_new();

  if (!sl)
    {
      btrace("failed to create new segment");
      return 1;
    }

  sr->rsmp.val  = sl->rsmp.val;
  sr->rsmp.fill = sl->rsmp.fill;
  sr->rseg      = sl->rseg;

  sr->lsmp.val  = zm;
  sr->lsmp.fill = sl->lsmp.fill;
  sr->lseg      = sl;

  sl->rsmp.val  = zm;
  sl->rseg      = sr;

  return 0;
}
