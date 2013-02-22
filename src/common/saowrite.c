/*
  saowrite.c

  writes sao gradient files
  
  J.J. Green 2011
  $Id: saowrite.c,v 1.6 2011/11/10 18:55:24 jjg Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include "saowrite.h"

static int writestop(double x, double v, FILE* st)
{
  return fprintf(st,"(%.5g,%.5g)",x,v);
}

static int sao_write_stream(FILE* st, sao_t* sao, const char* name)
{
  time_t t = time(NULL);

  fprintf(st,"# SAOimage color table\n");

  if (name) fprintf(st,"# %s\n",name);

  fprintf(st,"# created by cptutils %s\n",VERSION);
  fprintf(st,"# %s",ctime(&t));

  fprintf(st,"PSEUDOCOLOR\n");
  fprintf(st,"RED:\n");
  sao_eachred(sao,(stop_fn_t*)writestop,st); fprintf(st,"\n");
  fprintf(st,"GREEN:\n");
  sao_eachgreen(sao,(stop_fn_t*)writestop,st); fprintf(st,"\n");
  fprintf(st,"BLUE:\n");
  sao_eachblue(sao,(stop_fn_t*)writestop,st); fprintf(st,"\n");

  return 0;
}

extern int sao_write(const char* path, sao_t* sao, const char* name)
{
  int err = 0;

  if (path)
    {
      FILE* st = fopen(path,"w");

      if (!st) return 1;

      err = sao_write_stream(st,sao,name);

      fclose(st);
    }
  else
    {
      err = sao_write_stream(stdout,sao,name);
    }

  return err;
}


