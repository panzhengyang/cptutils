/*
  svgcpt.c : convert svg file to cpt file
 
  $Id: svgcpt.c,v 1.7 2005/06/17 00:25:27 jjg Exp jjg $
  J.J. Green 2005
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "colour.h"
#include "svgread.h"

#include "svgcpt.h"

static int svgcpt_list(svgcpt_opt_t,svg_list_t*);

extern int svgcpt(svgcpt_opt_t opt)
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
	  if (opt.list) err += svgcpt_list(opt,list);

	  /* select by id here */ 

	  /* select first here */

	}

      svg_list_destroy(list);
    }

  return err;
}

/* print the gradients in the list */ 

static int svg_id(svg_t*,const char*);

static int svgcpt_list(svgcpt_opt_t opt,svg_list_t* list)
{
  int n,err=0;

  n = svg_list_size(list);

  if (opt.verbose)
    {
      if (n==0)
	{
	  /* worrying, but not an error */

	  printf("no gradient found!\n");
	}
      else
	{
	  printf("found %i gradient%s\n",n,(n==1 ? "" : "s"));
	  err = svg_list_iterate(list,(int (*)(svg_t*,void*))svg_id,"  %s\n");
	}
    }
  else
    {
      err = svg_list_iterate(list,(int (*)(svg_t*,void*))svg_id,"%s\n");
    }

  if (err)  fprintf(stderr,"error listing svg\n");

  return err;
}

static int svg_id(svg_t* svg,const char* fmt)
{
  printf(fmt,svg->name);

  return 0;
}



     /*
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

*/
