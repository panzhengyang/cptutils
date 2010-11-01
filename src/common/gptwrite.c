/*
  gptwrite.c

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010
  $Id: povwrite.c,v 1.3 2005/09/21 22:00:07 jjg Exp $
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

      fprintf(st,"%7.5f %3i %3i %3i\n",
	      stop.z,
	      (int)stop.rgb[0],
	      (int)stop.rgb[1],
	      (int)stop.rgb[2]);
    }

  fclose(st);

  return 0;
}

