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

#include <unistd.h>

#include <btrace.h>

#include "options.h"
#include "cptcat.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  cptcat_opt_t opt = {0};

  /* use gengetopt */

  if (options(argc, argv, &info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_given;

  opt.output = (info.output_given ? info.output_arg : NULL);

  if (!opt.output && opt.verbose)
    {
      fprintf(stderr,"verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
    }

  if (info.inputs_num < 2)
    {
      fprintf(stderr,"at least 2 input files required\n");
      return EXIT_FAILURE;
    }

  opt.input.n    = info.inputs_num; 
  opt.input.file = (const char**)info.inputs;
  
  if (opt.verbose)
    printf("This is cptcat (version %s)\n",VERSION);

  btrace_enable("cptcat");

  int err = cptcat(opt);

  if (err)
    {
      btrace("failed (error %i)", err);
      btrace_print_stream(stderr, BTRACE_PLAIN);
      if (info.backtrace_file_given)
        btrace_print(info.backtrace_file_arg, BTRACE_XML);
    }

  btrace_reset();
  btrace_disable();

  if (opt.verbose)
    printf("done.\n");

  options_free(&info);

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

