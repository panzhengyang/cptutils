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

  $Id: main.c,v 1.7 2001/06/04 00:54:20 jjg Exp $
*/

#define _POSIX_C_SOURCE 199506L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "gimpcpt.h"
#include "colour.h"
#include "version.h"

static void printversion(FILE*);
static void printhelp(FILE*);

#define SAMPLES_DEFAULT 100
#define SAMPLES_MIN     5
#define NAN_DEFAULT     "255/0/0"
#define FG_DEFAULT      "255/255/255"
#define BG_DEFAULT      "0/0/0"
#define TRANS_DEFAULT   "255/255/255"
#define MINMAX_DEFAULT  "0.0/1.0"

const char versionstring[] = VERSION;
static int parse_minmax(char*,double*,double*);

int main(int argc,char** argv)
{
  int        err;
  char       arg, *infile=NULL, *outfile=NULL;
  cptopt_t   opt;

  /* defaulta */

  opt.verbose    = 0;
  opt.reverse    = 0;

  parse_rgb(NAN_DEFAULT, &opt.nan);
  parse_rgb(FG_DEFAULT, &opt.fg);
  parse_rgb(BG_DEFAULT, &opt.bg);
  parse_rgb(TRANS_DEFAULT,&opt.trans);

  parse_minmax(MINMAX_DEFAULT,&opt.min,&opt.max);

  opt.samples = SAMPLES_DEFAULT;

  while ((arg = getopt(argc,argv,"b:f:hn:o:r:s:t:Vv")) != -1)
  {
      switch (arg)
      {
	  case 'b':
	      if (parse_rgb(optarg,&opt.bg) != 0)
		  return EXIT_FAILURE;
	      break;
	  case 'f':
	      if (parse_rgb(optarg,&opt.fg) != 0)
		  return EXIT_FAILURE;
	      break;
	  case 'h':
	      printhelp(stdout);
	      return EXIT_SUCCESS;
	      break;
	  case 'n':
	      if (parse_rgb(optarg,&opt.nan) != 0)
		  return EXIT_FAILURE;
	      break;
	  case 'o':
	      outfile = optarg;
	      break;
	  case 'r':
	      if (parse_minmax(optarg,&opt.min,&opt.max) != 0)
	      {
		  fprintf(stderr,"error reading range \"%s\"\n",optarg);
		  return EXIT_FAILURE;
	      }
	      break;
	  case 's':
	      if ((opt.samples = atoi(optarg)) < SAMPLES_MIN)
	      {
		  fprintf(stderr,"at least %i samples required\n",SAMPLES_MIN);
		  opt.samples = SAMPLES_MIN;
	      }
	      break;
	  case 't':
	      if (parse_rgb(optarg,&opt.trans) != 0)
		  return EXIT_FAILURE;
	      break;
	  case 'V':
	      printversion(stdout);
	      return EXIT_SUCCESS;
	      break;
	  case 'v':
	      opt.verbose = 1;
	      break;

	  default:
	      printhelp(stderr);
	      return EXIT_FAILURE;
        }
    }

  if (argc - optind > 1)
    {
      fprintf(stderr,"Sorry, only one file at a time\n");
      return EXIT_FAILURE;
    }

  infile = argv[optind];

  if (!outfile && opt.verbose)
  {
      fprintf(stderr,"verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
  }

  if (opt.verbose)
    printf("This is gimpcpt (version %s)\n",versionstring);

  err = gimpcpt(infile,outfile,opt);

  if (opt.verbose)
    {
      if (err != 0)
        fprintf(stderr,"failed (error %i)\n",err);
      else
        printf("palette written to %s\n",(outfile ? outfile : "<stdin>"));
    }

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void printhelp(FILE* stream)
{
  fprintf(stream,"gimpcpt -- convert gimp gradient files to GMT palattes\n");
  fprintf(stream,"usage: gimpcpt [options] file\n");
  fprintf(stream,"  -b <col>   set background colour to <col> [%s]\n",BG_DEFAULT);
  fprintf(stream,"  -h         this help\n");
  fprintf(stream,"  -f <col>   set foreground colour to <col> [%s]\n",FG_DEFAULT);
  fprintf(stream,"  -n <col>   set NaN colour to <col> [%s]\n",NAN_DEFAULT);
  fprintf(stream,"  -o <file>  set output to <file>\n");
  fprintf(stream,"  -r <range> set range to min/max [%s]\n",MINMAX_DEFAULT);
  fprintf(stream,"  -t <col>   replace Gimp transparency by <col> [%s]\n",TRANS_DEFAULT);
  fprintf(stream,"  -s <samp>  use at most <samp> samples [%i]\n",SAMPLES_DEFAULT);
  fprintf(stream,"  -v         verbose operation\n");
  fprintf(stream,"  -V         version information\n");
}

static void printversion(FILE* stream)
{
    fprintf(stream,"gimpcpt version %s\n",versionstring);
    fprintf(stream,"compiled at %s on %s\n",__TIME__,__DATE__);
}
    	
static int parse_minmax(char* s,double* min,double* max)
{
    *min = atof(s);
    if ((s = strchr(s,'/')) == NULL) return 1; 
    *max = atof(++s); 
    	
    return 0;
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

