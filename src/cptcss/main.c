/*
  main.c for cptcss

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

#include "options.h"
#include "cptcss.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  cptcss_opt_t opt;
  int err;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = (info.verbose_given ? true : false);

  if (opt.verbose)
    printf("This is cptcss (version %s)\n",VERSION);
  
  switch (info.inputs_num)
    {
    case 0:
      opt.cpt = NULL;
      break;
    case 1:
      opt.cpt = info.inputs[0];
      break;
    default:
      fprintf(stderr,"Sorry, only one file at a time\n");
      return EXIT_FAILURE;
    }

  opt.format = (info.format_given ? info.format_arg : "cptcss-%.2i");

  err = cptcss(opt);
  
  if (err != 0) fprintf(stderr,"failed (error %i)\n",err);

  if (opt.verbose) printf("done.\n");

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

