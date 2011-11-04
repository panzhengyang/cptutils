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
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.

  $Id: main.c,v 1.7 2011/11/04 13:11:32 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "version.h"
#include "options.h"
#include "avlcpt.h"


int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  avlcpt_opt_t opt;
  char *infile,*outfile;
  int err;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_given;
  opt.debug   = info.debug_given;

  opt.adjust.lower = (info.lower_given ? info.lower_arg : 0.0);
  opt.adjust.upper = (info.upper_given ? info.upper_arg : 0.0);

  /* precision */

  opt.precision = (info.precision_given ? info.precision_arg : 1.0);
  
  if ( ! (opt.precision > 0))
    {
      fprintf(stderr,"precision must be positive but %f isn't\n",opt.precision);
      return EXIT_FAILURE;
    }

  /* null outfile for stdout */

  outfile = (info.output_given ? info.output_arg : NULL);

  if (!outfile && opt.verbose)
    {
      fprintf(stderr,"verbosity suppressed (<stdout> for results)\n");
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
    printf("This is avlcpt (version %s)\n",VERSION);
  
  opt.file.input  = infile;
  opt.file.output = outfile;

  /* get fg/bg/nan */

  if (parse_rgb(info.background_arg,&opt.bg) != 0)
    {
      fprintf(stderr,"bad background %s\n",info.background_arg);
      return EXIT_FAILURE;
    }

  if (parse_rgb(info.foreground_arg,&opt.fg) != 0)
    {
      fprintf(stderr,"bad foreground %s\n",info.foreground_arg);
      return EXIT_FAILURE;
    }

  if (parse_rgb(info.nan_arg,&opt.nan) != 0)
    {
      fprintf(stderr,"bad nan colour %s\n",info.nan_arg);
      return EXIT_FAILURE;
    }

  err = avlcpt(opt);

  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);

      printf("done.\n");
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

