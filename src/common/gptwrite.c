/*
  gptwrite.c

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010
  $Id: gptwrite.c,v 1.1 2010/11/01 18:42:43 jjg Exp jjg $
*/

#include <stdlib.h>
#include <stdio.h>

#include "gpt.h"
#include "version.h"

extern int gpt_write(const char* file,gpt_t* gpt)
{
  FILE *st;
  int n,i;

  n = gpt->n;

  if ( n<2 )
    {
      fprintf(stderr,"gnuplot does not support %i stops\n",n);
      return 1;
    }

  if (file)
    {
      st = fopen(file,"w");
      if (!st) return 1;
    }
  else st = stdout;

  for (i=0 ; i<n ; i++)
    {
      gpt_stop_t stop = gpt->stop[i];

      fprintf(st,"%7.5f %7.5f %7.5f %7.5f\n",
	      stop.z,
	      stop.rgb[0],
	      stop.rgb[1],
	      stop.rgb[2]);
    }

  fclose(st);

  return 0;
}

