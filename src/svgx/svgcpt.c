/*
  svgcpt.c : convert svg file to cpt file
 
  $Id: svgcpt.c,v 1.6 2005/06/15 22:42:32 jjg Exp jjg $
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

static int svg_id(svg_t*,const char*);

static int svgcpt_stream(svgcpt_opt_t opt)
{
  int err = 0;
  svg_list_t *list;

  if ((list = svg_list_new()) == NULL)
    {
      fprintf(stderr,"error creating new list\n");
      err++;
    }
  else
    {
      if (svg_read(opt.file.input,list) != 0)
	{
	  fprintf(stderr,"error reading svg\n");
	  err++;
	}
      else
	{
	  const char 
	    vfmt[] = " - %s\n", 
	    pfmt[] = "%s\n";

	  if (svg_list_iterate(list,
			       (int (*)(svg_t*,void*))svg_id,
			       (void*)(opt.verbose ? vfmt : pfmt)) != 0)
	    { 
	      fprintf(stderr,"error converting svg\n");
	      err++;
	    }
	}

      svg_list_destroy(list);
    }

  return err;
}

static int svg_id(svg_t* svg,const char* fmt)
{
  printf(fmt,svg->name);

  return 0;
}
