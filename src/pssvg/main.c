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

#include <btrace.h>

#include "options.h"
#include "pssvg.h"

int main(int argc, char** argv)
{
  struct gengetopt_args_info info;
  pssvg_opt_t opt = {0};
  char *infile, *outfile;

  /* gengetopt */

  if (options(argc, argv, &info) != 0)
    {
      fprintf(stderr, "failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_flag;

  /* title format string */

  opt.title = (info.title_given ? info.title_arg : NULL);

  /* fore/background */

  if (parse_rgb(info.background_arg, &(opt.bg)) != 0)
    {
      fprintf(stderr,"bad background %s\n",info.background_arg);
      return EXIT_FAILURE;
    }

  if (parse_rgb(info.foreground_arg, &(opt.fg)) != 0)
    {
      fprintf(stderr,"bad foreground %s\n",info.foreground_arg);
      return EXIT_FAILURE;
    }

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

  if (!outfile && opt.verbose)
   {
      fprintf(stderr, "verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
    }

  /* say hello */

  if (opt.verbose)
    {
      printf("This is pssvg (version %s)\n", VERSION);
      if (opt.title)
	printf("with title format '%s'\n", opt.title);
      printf("background %3i/%3i/%3i\n", 
	     opt.bg.red,  opt.bg.green,  opt.bg.blue);
      printf("foreground %3i/%3i/%3i\n", 
	     opt.fg.red,  opt.fg.green,  opt.fg.blue);
    }

  btrace_enable();

  int err = pssvg(opt);

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
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

