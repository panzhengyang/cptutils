/*
  cptinfo.h - summary information on a cpt file
  J.J. Green 2004
  $Id: cptinfo.c,v 1.4 2004/03/21 22:22:22 jjg Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cptio.h"

#include "cpttext.h"

#define NNSTR(x) ((x) ? (x) : "<stdin>")

static int output_html(char*,cpt_t*,double*);

extern int cpttext(cpttext_opt_t opt)
{
  cpt_t *cpt;
  double z[2];

  if ((cpt = cpt_new()) == NULL)
    {
      fprintf(stderr,"failed to initialise cpt structure\n");
      return 1;
    }

  if (cpt_read(opt.cpt,cpt,0) != 0)
    {
      fprintf(stderr,"failed to read %s\n",NNSTR(opt.cpt));
      return 1;
    }

  if (cpt_zrange(cpt,z) != 0)
    {
      fprintf(stderr,"failed to get zrange\n");
      return 1;
    }

  switch (opt.format)
    {
    case html:
      if (output_html(opt.text,cpt,z) != 0)
	{
	  fprintf(stderr,"error in html output\n");
	  return 1;
	}
      break;

   default:
      fprintf(stderr,"strange output specified\n");
      return 1;
    }

  cpt_destroy(cpt);

  return 0;
}

static int output_html(char *text,cpt_t *cpt,double *zrng)
{
  int n,i;
  double dz,z;

  n = strlen(text);

  dz = (zrng[1]-zrng[0])/n;

  printf("%f %f\n",zrng[0],zrng[1]);

  for (i=0 ; i<n ; i++)
    {
      fill_t fill;
      char   hcstr[8]; 

      z = zrng[0] + (0.5 + i)*dz;

      if (cpt_zfill(cpt,z,&fill) != 0)
	{
	  fprintf(stderr,"error getting fill for z = %f\n",z);
	  return 1;
	}

      printf("<font colour=\"%s\">%c</font>",hcstr,text[i]);
    }

  return 0;
}

