/*
  main.c 

  part of the gimpcpt package

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

  $Id: main.c,v 1.6 2010/06/24 19:43:04 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "xycpt.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  xycpt_opt_t opt;
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
    printf("This is xycpt (version %s)\n",VERSION);
  
  opt.file.input  = infile;
  opt.file.output = outfile;

  if (info.register_given)
    {
      if ((opt.discrete = info.discrete_given) != 0)
	{
	  switch (*info.register_arg)
	    {
	    case 'l': case 'L': 
	      opt.reg = reg_lower;
	      break;
	    case 'm': case 'M':
	      opt.reg = reg_middle;
	      break;
	    case 'u': case 'U': case 'h': case 'H':
	      opt.reg = reg_upper;
	      break;
	    default:
	      fprintf(stderr,
		      "unknown registration type \"%s\"\n",
		      info.register_arg);
	      return EXIT_FAILURE;
	    }
	}
      else
	{
	  fprintf(stderr,"registration only applies to discrete output!\n");
	  fprintf(stderr,"(drop the -r option or use the -d option)\n");
	  return EXIT_FAILURE;
	}
    }
  else
    {
      opt.discrete = info.discrete_given;
      opt.reg = reg_middle;
    }

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

  opt.unital = info.unit_given;

  if (opt.verbose)
    {
      if (opt.discrete)
	{
	  const char* regstr;
	  
	  switch (opt.reg)
	    {
	    case reg_lower  : regstr = "lower"  ; break;
	    case reg_middle : regstr = "middle" ; break;
	    case reg_upper  : regstr = "upper"  ; break;
	    default:
	      fprintf(stderr,"bad registration\n");
	      return EXIT_FAILURE;
	    }

	  printf("converting to discrete palette (%s registration)\n",regstr);
	}
    }

  err = xycpt(opt);

  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);

      printf("done.\n");
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

