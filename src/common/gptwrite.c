/*
  gptwrite.c

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "gpt.h"
#include "btrace.h"

extern int gpt_write(const char* file,gpt_t* gpt)
{
  int n,i;

  n = gpt->n;

  if ( n<2 )
    {
      btrace("gnuplot does not support %i stops", n);
      return 1;
    }

  FILE *st = NULL;

  if (file)
    {
      if ( ! ( st = fopen(file, "w") ))
	{
	  btrace("failed to open file '%s' for write", file);
	  return 1;
	}
    }
  else st = stdout;

  time_t t = time(NULL);

  fprintf(st, "# Gnuplot colour map\n");
  fprintf(st, "# cptutils %s, %s", VERSION, ctime(&t));
  
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

