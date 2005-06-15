/*
  svgcpt.c : convert svg file to cpt file
 
  $Id$
  J.J. Green 2005
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "colour.h"
#include "svgread.h"

#include "svgcpt.h"

static int svgcpt_stream(svgcpt_opt_t);

extern int svgcpt(svgcpt_opt_t opt)
{
  int err=0;

  if (opt.file.output)
    {
      FILE *stream;
      
      stream = fopen(opt.file.output,"w");
      
      if (stream == NULL)
	{
	  fprintf(stderr,"error opening file %s : %s\n",
		  opt.file.output,
		  strerror(errno));
	  err = 1;
	}
      else
	{
	  opt.stream.output = stream;
	  err = svgcpt_stream(opt); 
	  fclose(stream);
	}
    }
  else
    {
      opt.stream.output = stdout;
      err = svgcpt_stream(opt); 
    } 

  return err;
}

static int svgcpt_stream(svgcpt_opt_t opt)
{
  int err;
  svg_list_t *list;

  if ((list = svg_list_new()) == NULL)
    {
      fprintf(stderr,"error creating new list\n");
      return 1;
    }

  err = svg_read(opt.file.input,list);
    
  svg_list_destroy(list);

  return err;
}

