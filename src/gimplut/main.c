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

  $Id: main.c,v 1.4 2007/01/25 00:12:58 jjg Exp $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "options.h"
#include "gimplut.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  char   *infile=NULL,*outfile=NULL;
  int     err;
  glopt_t opt;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_given;

  opt.numsamp = info.samples_arg;

  if (opt.numsamp < 1)
    {
      fprintf(stderr,"bad number of samples %i\n",opt.numsamp);
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
    printf("This is gimplut (version %s)\n",VERSION);
  
  err = gimplut(infile,outfile,opt);

  if (err != 0)
    fprintf(stderr,"failed (error %i)\n",err);
  else
    {
      if (opt.verbose)
	{
	  printf("lookup table written to %s\n",(outfile ? outfile : "<stdin>"));
	  printf("done.\n");
	}
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

