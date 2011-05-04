/*
  css3write.c

  write a gnuplot colour map struct to
  a file or stream.

  J.J.Green 2010
  $Id: css3write.c,v 1.4 2010/11/01 19:45:12 jjg Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "css3.h"
#include "version.h"

extern int css3_write(const char* file,css3_t* css3)
{
  FILE *st;
  int n,i;

  n = css3->n;

  if ( n<2 )
    {
      fprintf(stderr,"CSS3 does not support %i stops\n",n);
      return 1;
    }

  if ( file )
    {
      st = fopen(file,"w");
      if (!st) return 1;
    }
  else st = stdout;

  time_t t = time(NULL);

  fprintf(st,"/*\n");
  fprintf(st,"   CSS3 gradient\n");
  fprintf(st,"   cptutils %s\n",VERSION);
  fprintf(st,"   %s",ctime(&t));
  fprintf(st,"*/\n");
  fprintf(st,"\n");
  fprintf(st,"linear-gradient(\n");
  fprintf(st,"  %.3gdeg,\n",css3->angle);
  
  for (i=0 ; i<n ; i++)
    {
      css3_stop_t stop = css3->stop[i];

      fprintf(st,"  rgb(%3i,%3i,%3i) %7.3f%%%s\n",
	      stop.rgb.red,
	      stop.rgb.green,
	      stop.rgb.blue,
	      stop.z,
	      (n-1-i ? "," : "")
	      );
    }

  fprintf(st,"  );\n");

  fclose(st);

  return 0;
}

