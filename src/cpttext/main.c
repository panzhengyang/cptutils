/*
  main.c for cpttext

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
  Free Software Foundation, Inc., Copyright (C) 1989, 1991 
  Free Software Foundation, Inc.,

  $Id: main.c,v 1.4 2012/02/26 22:27:25 jjg Exp $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "cpttext.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  cpttext_opt_t opt;
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
    printf("This is cpttext (version %s)\n",VERSION);
  
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

  opt.text   = info.text_arg;
  opt.format = html;
      
  err = cpttext(opt);
  
  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);

      printf("done.\n");
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

