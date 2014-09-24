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

#include <colour.h>
#include <btrace.h>

#include "options.h"
#include "cpthsv.h"

static int parse_transforms(const char*,cpthsv_opt_t*);

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  char    *infile = NULL, *outfile=NULL;
  cpthsv_opt_t opt = {0};

  /* use gengetopt */

  if (options(argc, argv, &info) != 0)
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
      fprintf(stderr,"Sorry, only one file at a time\n");
      return EXIT_FAILURE;
    }
  
  if (opt.verbose)
    printf("This is cpthsv (version %s)\n",VERSION);
  
  if (! info.transform_given )
    {
      fprintf(stderr,"no transform specified, nothing to do\n");
      return EXIT_FAILURE;
    }

  if (parse_transforms(info.transform_arg, &opt) != 0)
    {
      fprintf(stderr,"failed to parse transform %s\n",
	      info.transform_arg);
      return EXIT_FAILURE;
    }

  btrace_enable();

  int err = cpthsv(infile, outfile, opt);

  if (err)
    {
      btrace("failed (error %i)", err);
      btrace_print_plain(stderr);
      if (info.backtrace_file_given)
        btrace_print(info.backtrace_file_arg, BTRACE_XML);
    }
  else if (opt.verbose)
    printf("gradient written to %s\n", (outfile ? outfile : "<stdin>"));

  btrace_reset();
  btrace_disable();

  if (opt.verbose)
    printf("done.\n");

  options_free(&info);
  
  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}

static int parse_transform(char*,hsvtrans_t*,cpthsv_opt_t*);    	

static int parse_transforms(const char* T,cpthsv_opt_t* opt)
{
  int n = 1;
  char *v;
  const char *c;

  for (c=T ; *c ; c++) n += (*c == ',');

  int i;
  char *Tdup = strdup(T);
  hsvtrans_t *t = malloc(n*sizeof(hsvtrans_t));

  v = strtok(Tdup,",");  
  if (parse_transform(v,t,opt) != 0)
    {
      fprintf(stderr,"failed parse of %s\n",v);
      return 1;
    }

  for (i=1 ; i<n ; i++)
    {
      v = strtok(NULL,",");  
      if (parse_transform(v,t+i,opt) != 0)
	{
	  fprintf(stderr,"failed parse of %s\n",v);
	  return 1;
	}
    }

  if (opt->verbose)
    printf("read %i transforms\n",n);

  opt->n    = n;
  opt->tran = t;

  return 0;
}    	
    	
static int parse_transform(char* T,hsvtrans_t* t,cpthsv_opt_t* opt)
{
  char cc,co;
  double z;

  switch (sscanf(T,"%c%lf%c",&cc,&z,&co))
    {
    case 0: 
    case 1:
      return 1;
    case 2:
      co = '%';
    case 3:

      t->z = z;

      switch (tolower(cc))
	{
	case 'h': t->channel = hue; break;
	case 's': t->channel = saturation; break;
	case 'v': t->channel = value; break;
	default:
	  fprintf(stderr,"bad channel %c\n",*T);
	  return 1;
	}

      switch (tolower(co))
	{
	case '%': t->op = percent; break;
	case 'x': t->op = times; break;
	case '+': t->op = plus; break;
	case '-': t->op = minus; break;
	default:
	  fprintf(stderr,"bad operation %c\n",*T);
	  return 1;
	}

      break;

    default:
      return 1;
    }

  return 0;
}
    	
    	
    	
    	
    	
    	
    	

