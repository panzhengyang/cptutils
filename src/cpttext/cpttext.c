/*
  cpttext.c - colour text with a cpt file

  J.J. Green 2004
  $Id: cpttext.c,v 1.1 2004/04/12 23:43:01 jjg Exp jjg $
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
  int    err = 0;

  if ((cpt = cpt_new()) == NULL)
    fprintf(stderr,"failed to initialise cpt structure\n");
  else
    {
      if (cpt_read(opt.cpt,cpt,0) != 0)
	{
	  fprintf(stderr,"failed to read %s\n",NNSTR(opt.cpt));
	  err = 1;
	  goto cleanup;
	}

      if (cpt_zrange(cpt,z) != 0)
	{
	  fprintf(stderr,"failed to get zrange\n");
	  err = 1;
	  goto cleanup;
	}

      switch (opt.format)
	{
	case html:
	  if (output_html(opt.text,cpt,z) != 0)
	    {
	      fprintf(stderr,"error in html output\n");
	      err = 1;
	      goto cleanup;
	    }
	  break;
	  
	case css:
	  fprintf(stderr,"css output not yet implemented\n");
	  err = 1;
	  goto cleanup;
	  break;
	  
	default:
	  fprintf(stderr,"strange output specified\n");
	  err = 1;
	  goto cleanup;
	}
      
    cleanup:
  
      cpt_destroy(cpt);
    }

  return err;
}

static int output_html(char *text,cpt_t *cpt,double *zrng)
{
  int     n,i;
  double  dz,z;
  model_t model;

  n = strlen(text);

  dz = (zrng[1]-zrng[0])/n;

  model = cpt->model;

#ifdef DEBUG
  printf("%f %f\n",zrng[0],zrng[1]);
#endif

  for (i=0 ; i<n ; i++)
    {
      fill_t fill;
      char   hcstr[8]; 
      rgb_t  rgb;
      char   c = text[i];

      z = zrng[0] + (0.5 + i)*dz;

      if (cpt_zfill(cpt,z,&fill) != 0)
	{
	  fprintf(stderr,"error getting fill for z = %f\n",z);
	  return 1;
	}

      if (fill_rgb(fill,model,&rgb) != 0)
	{
	  fprintf(stderr,"error rgb values for fill (z = %f)\n",z);
	  return 1;
	}

#ifdef DEBUG
      printf("%f -> (%i %i %i) ",z,rgb.red,rgb.blue,rgb.green);
#endif

      switch (c)
	{
	case ' ':
	case '\t':
	case '\n':
	  printf("%c",c);
	  break;
	default:
	  sprintf(hcstr,"#%.2x%.2x%.2x",rgb.red,rgb.blue,rgb.green);
	  printf("<font color=\"%s\">%c</font>",hcstr,c);
	}
    }

  printf("\n");

  return 0;
}

