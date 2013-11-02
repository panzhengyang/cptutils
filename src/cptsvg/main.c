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
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "cptsvg.h"

#include "colour.h"
#include "svg.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  char    *infile = NULL, *outfile = NULL;
  int      err;
  cptsvg_opt_t opt;

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
      fprintf(stderr,"Sorry, only one file at a time\n");
      return EXIT_FAILURE;
    }
  
  if (opt.verbose)
    printf("This is cptsvg (version %s)\n",VERSION);

  /* handle preview */

  if (info.preview_flag || info.geometry_given)
    {
      opt.preview.use = true;
      if (svg_preview_geometry(info.geometry_arg, &(opt.preview)) != 0)
	{
	  fprintf(stderr,"failed parse of geometry : %s\n",
		  info.geometry_arg);
	  return EXIT_FAILURE;
	}
    }
  else  
    opt.preview.use = false;

  svg_srand();

  err = cptsvg(infile, outfile, opt);
  
  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);
      else
	{
	  printf("gradient written to %s\n",(outfile ? outfile : "<stdin>"));
	  if (opt.preview.use)
	    {
	      printf("with preview (%zu x %zu px)\n", 
		     opt.preview.width,
		     opt.preview.height);
	    }
	}

      printf("done.\n");
    }
  
  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

