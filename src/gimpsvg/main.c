/*
  main.c 

  part of the gimpsvg package

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

  $Id: main.c,v 1.6 2011/11/10 23:31:44 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "gimpsvg.h"
#include "colour.h"

#define SAMPLES_MIN 5

static int parse_minmax(char*,double*,double*);

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  char *infile=NULL, *outfile=NULL;
  int err;
  gimpsvg_opt_t opt;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose = info.verbose_given;
  opt.reverse = 0;

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

  if (parse_rgb(info.transparency_arg,&opt.trans) != 0)
    {
      fprintf(stderr,"bad transparency %s\n",info.transparency_arg);
      return EXIT_FAILURE;
    }

  if (parse_minmax(info.range_arg,&opt.min,&opt.max) != 0)
    {
      fprintf(stderr,"bad range %s\n",info.range_arg);
      return EXIT_FAILURE;
    }

  if ((opt.samples = info.samples_arg) < SAMPLES_MIN)
    {
      fprintf(stderr,"at least %i samples required\n",SAMPLES_MIN);
      opt.samples = SAMPLES_MIN;
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
    printf("This is gimpsvg (version %s)\n",VERSION);
  
  err = gimpsvg(infile,outfile,opt);
  
  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);
      else
        printf("palette written to %s\n",(outfile ? outfile : "<stdin>"));
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}

static int parse_minmax(char* s,double* min,double* max)
{
    *min = atof(s);
    if ((s = strchr(s,'/')) == NULL) return 1; 
    *max = atof(++s); 
    	
    return 0;
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

