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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

#include <btrace.h>

#include "options.h"
#include "cptcont.h"

int main(int argc, char** argv)
{
  struct gengetopt_args_info info;
  char    *infile = NULL, *outfile = NULL;
  cptcont_opt_t opt = {0};

  /* use gengetopt */

  if (options(argc, argv, &info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose  = info.verbose_given;
  opt.midpoint = info.midpoint_given;

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
      fprintf(stderr, "Sorry, only one file at a time\n");
      return EXIT_FAILURE;
    }
  
  if (opt.verbose)
    printf("This is cptcont (version %s)\n", VERSION);
  
  opt.partial = (info.partial_given ? info.partial_arg/100.0 : 1.0);

  btrace_enable("cptcont");

  int err = cptcont(infile, outfile, opt);

  if (err)
    {
      btrace("failed (error %i)", err);
      btrace_print_stream(stderr, BTRACE_PLAIN);

      if (info.backtrace_file_given)
	{
	  int format = BTRACE_XML;
	  if (info.backtrace_format_given)
	    {
	      format = btrace_format(info.backtrace_format_arg);
	      if (format == BTRACE_ERROR)
		{
		  fprintf(stderr, "no such backtrace format %s\n", 
			  info.backtrace_format_arg);
		  return EXIT_FAILURE;
		}
	    }	  
	  btrace_print(info.backtrace_file_arg, format);
	}
    }
  else
    {
      if (opt.verbose)
	printf("gradient written to %s\n",(outfile ? outfile : "<stdin>"));
    }

  btrace_reset();
  btrace_disable();

  if (opt.verbose)
    printf("done.\n");

  options_free(&info);
  
  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	

