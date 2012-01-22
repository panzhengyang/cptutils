/*
  main.c 

  part of the cptutils package

  This program is free software; you can redistribute it
  and/or modify it under the terms of the GNU General
  Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A
  PARTICULAR PURIGHTE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public
  License along with this program; if not, write to the
  Free Software Foundation, Inc.,  51 Franklin Street, Fifth Floor, 
  Boston, MA 02110-1301 USA

  $Id: main.c,v 1.6 2012/01/22 19:39:13 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

#include "options.h"
#include "cptclip.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  char    *infile=NULL,*outfile=NULL;
  int      err;
  cptclip_opt_t opt;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_given;

  /* null outfile for stdout */

  outfile = (info.output_given ? info.output_arg : NULL);

  if (!outfile && opt.verbose)
    {
      fprintf(stderr,"verbosity suppressed (<stdout> is used for results)\n");
      opt.verbose = 0;
    }

  /* say hello */
  
  if (opt.verbose)
    printf("This is cptclip (version %s)\n",VERSION);

  /* null infile for stdin */

  switch (info.inputs_num)
    {
    case 0:
      infile = NULL;
      break;

    case 1:
      infile = info.inputs[0];
      break;

    default:
      fprintf(stderr,"expected 0 or 1 arguments, got %i\n",
	      info.inputs_num);
      return EXIT_FAILURE;
    }
  
  /* segments option */

  opt.segments = info.segments_given;

  if (opt.segments)
    {
      if (sscanf(info.range_arg,"%zu/%zu",
		 &opt.u.segs.min,
		 &opt.u.segs.max) != 2)
	{
	  fprintf(stderr,"bad range argument %s\n",info.range_arg);
	  return EXIT_FAILURE;
	}

      if ((opt.u.segs.min > opt.u.segs.max) ||
	  ( opt.u.segs.min < 1 ) || 
	  ( opt.u.segs.max < 1 )) 
	{
	  fprintf(stderr,"bad segment selection %zi - %zi\n",
		  opt.u.segs.min,opt.u.segs.max);
	  return EXIT_FAILURE;
	}

      if (opt.verbose)
	{
	  printf("extracting segments %zi to %zi\n",
		 opt.u.segs.min,opt.u.segs.max);
	}
    }
  else
    {
      if (sscanf(info.range_arg,"%lf/%lf",
		 &opt.u.z.min,
		 &opt.u.z.max) != 2)
	{
	  fprintf(stderr,"bad range argument %s\n",info.range_arg);
	  return EXIT_FAILURE;
	}

      if (opt.u.z.min >= opt.u.z.max) 
	{
	  fprintf(stderr,"bad range %g - %g\n",
		  opt.u.z.min,opt.u.z.max);
	  return EXIT_FAILURE;
	}

      if (opt.verbose)
	{
	  printf("extracting z-range %g to %g\n",
		 opt.u.z.min,opt.u.z.max);
	}

    }

  err = cptclip(infile,outfile,opt);

  if (err) fprintf(stderr,"failed (error %i)\n",err);

  if (opt.verbose)
    {
      printf("gradient written to %s\n",(outfile ? outfile : "<stdin>"));
      printf("done.\n");
    }
  
  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
