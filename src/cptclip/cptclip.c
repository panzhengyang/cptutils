/*
  cptclip.c

  (c) J.J.Green 2010
  $Id: cptclip.c,v 1.2 2010/04/12 21:36:32 jjg Exp jjg $
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

static int cptclip_z(cpt_t*,cptclip_opt_t);

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

  int err = 0;

#if 0

  // FIXME - need to implement the below

  if (cpt_increasing(cpt))
    err = cptclip_z(cpt,opt);
  else
    {
      cpt_reverse(cpt);
      err = cptclip_z(cpt,opt);
      cpt_reverse(cpt);
    }

#endif

  return err;
}

static int cptclip_z(cpt_t* cpt,cptclip_opt_t opt)
{
  // FIXME

  return 0;
}
