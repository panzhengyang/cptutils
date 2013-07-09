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
#include "cptcat.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  cptcat_opt_t opt = {0};
  int err;

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

  /* get fg/bg/nan */

  struct 
  {
    int given;
    char *str;
    rgb_t *rgb;
    const char *descr;
  } rgb_data[3] = 
      {
	{info.background_given, info.background_arg, opt.bg,  "background"},
	{info.foreground_given, info.foreground_arg, opt.fg,  "foreground"},
	{info.nan_given,        info.nan_arg,        opt.nan, "NaN"}
      }, *dat;

  int i;

  for (i=0 ; i<3 ; i++)
    {
      dat = rgb_data + i;
      if (dat->given)
	{
	  if (parse_rgb(dat->str, dat->rgb) != 0)
	    {
	      fprintf(stderr, "bad %s %s\n", dat->descr, dat->str);
	      return EXIT_FAILURE;
	    }
	}
    }

  err = cptcat(opt);

  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n", err);
      printf("done.\n");
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

