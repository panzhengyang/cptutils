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
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
  Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "pssvg.h"

int main(int argc, char** argv)
{
  struct gengetopt_args_info info;
  pssvg_opt_t opt;
  char *infile, *outfile;
  int err;

  /* use gengetopt */

  if (options(argc, argv, &info) != 0)
    {
      fprintf(stderr, "failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose    = info.verbose_flag;
  opt.name       = NULL;

  /* null outfile for stdout */

  outfile = (info.output_given ? info.output_arg : NULL);

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
      fprintf(stderr, 
	      "Exactly one Photoshop gradient file must be specified!\n");
      options_print_help();
      return EXIT_FAILURE; 
    }
  
  opt.file.input  = infile;
  opt.file.output = outfile;

  /* 
     we write the translation of the svg gradient <name> to stdout 
     if <name> is specified, so then we suppress verbosity
  */

  if (opt.name && !outfile && opt.verbose)
   {
      fprintf(stderr, "verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
    }

  /* say hello */

  if (opt.verbose)
    printf("This is pssvg (version %s)\n", VERSION);

  /* for conversion, give details of what we will do */

  err = pssvg(opt);

  if (err) fprintf(stderr, "failed (error %i)\n", err);

  if (opt.verbose) printf("done.\n");

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

