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
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.

  $Id: main.c,v 1.4 2005/06/23 23:14:52 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "svgx.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  svgx_opt_t opt;
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
  opt.list    = info.list_given;
  opt.all     = info.all_given;
  opt.name    = (info.name_given ? info.name_arg : NULL);

  if (info.type_given)
    {
      const char* tstr = info.type_arg;

      if (strcmp("cpt",tstr) == 0)
	opt.type = type_cpt;
      else if (strcmp("ggr",tstr) == 0)
	opt.type = type_ggr;
      else
	{
	  fprintf(stderr,"no such type %s\n",tstr);
	  return EXIT_FAILURE;
	}
    }
  else
    opt.type = type_cpt;

  /* null outfile for stdout */

  outfile = (info.output_given ? info.output_arg : NULL);

  /* null infile for stdin */

  switch (info.inputs_num)
    {
    case 1:
      infile = info.inputs[0];
      break;
    default:
      fprintf(stderr,"Exactly one SVG file must be specified\n");
      options_print_help();
      return EXIT_FAILURE;
    }
  
  opt.input.file  = infile;
  opt.output.file = outfile;

  /* 
     we write the translation of the svg gradient <name> to stdout 
     if <name> is specified, so then we suppress verbosity
  */

  if (opt.name && !outfile && opt.verbose)
    {
      fprintf(stderr,"verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
    }

  if (opt.verbose)
    {
      printf("This is svgx (version %s)\n",VERSION);

      if (opt.name || opt.all)
	{
	  const char* tstr;

	  switch (opt.type)
	    {
	    case type_cpt : tstr = "cpt"; break;
	    case type_ggr : tstr = "gimp"; break;
	    default:
	      fprintf(stderr,"wierd output format!\n");
	      return EXIT_FAILURE;
	    }
	  
	  printf("mode is svg%s\n",tstr);
	}
    }

  err = svgx(opt);

  if (err != 0)
    fprintf(stderr,"failed (error %i)\n",err);

  if (opt.verbose)
    printf("done.\n");

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

