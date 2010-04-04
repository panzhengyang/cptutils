/*
  cptcont.c

  (c) J.J.Green 2010
  $Id: cptcont.c,v 1.1 2010/04/04 17:27:27 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cptio.h"

#include "cptcont.h"

static int cptcont_convert(cpt_t*,cptcont_opt_t);

extern int cptcont(char* infile,char* outfile,cptcont_opt_t opt)
{
  cpt_t *cpt;
  int err=0;

  if ((cpt = cpt_new()) != NULL)
    {
      if (cpt_read(infile,cpt,0) == 0)
	{
	  if (cptcont_convert(cpt,opt) == 0)
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

static int cptcont_convert(cpt_t* cpt,cptcont_opt_t opt)
{
  /* check we have at least one cpt segment */

  if (cpt->segment == NULL)
    {
      fprintf(stderr,"cpt has no segments\n");
      return 1;
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
          fprintf(stderr,"sorry, can't convert mixed fill types\n");
          return 1;
        }

      if (! fill_eq(s1->rsmp.fill,s2->lsmp.fill))
	{
	  fill_t F1,F2;
	  int err;

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
	      fprintf(stderr,"failed fill intepolate\n");
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
