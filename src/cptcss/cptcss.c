/*
  cptcss.c - colour text with a cpt file

  J.J. Green 2004
  $Id: cptcss.c,v 1.1 2005/07/19 22:17:56 jjg Exp jjg $
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cptio.h"

#include "cptcss.h"

#define NNSTR(x) ((x) ? (x) : "<stdin>")

static int output_css(cpt_t*,const char*);

extern int cptcss(cptcss_opt_t opt)
{
  cpt_t *cpt;
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

      if (output_css(cpt,opt.format) != 0)
	{
	  fprintf(stderr,"error in css output\n");
	  err = 1;
	  goto cleanup;
	}
      
    cleanup:
  
      cpt_destroy(cpt);
    }

  return err;
}

static int output_css(cpt_t *cpt,const char *format)
{
  int n = 0;
  model_t model;
  cpt_seg_t *seg;
  int cllen;

  seg   = cpt->segment;
  model = cpt->model;

  /* 
     needs to fit the format string, might get an overflow
     for sufficiently hostile format string, "arse-%.25" 
     for example
   */

  cllen = strlen(format) + 20;

  while (seg)
    {
      fill_t fill;
      rgb_t  rgb;
      char   hcstr[8]; 
      char   clstr[cllen];

      if (! fill_eq(seg->lsmp.fill,seg->rsmp.fill))
	{
	  fprintf(stderr,"non-discrete cpt file! (use makecpt)\n");
	  return 1;
	}

      fill = seg->lsmp.fill;

      if (fill_rgb(fill,model,&rgb) != 0)
	{
	  fprintf(stderr,"failed to get rgb!\n");
	  return 1;
	}
      
      sprintf(hcstr,"#%.2x%.2x%.2x",rgb.red,rgb.green,rgb.blue);
      
      if (snprintf(clstr,cllen,format,n) > cllen-1)
	{
	  fprintf(stderr,"output truncated (hostile format string?)\n");
	  return 1;
	}
      
      printf("td.%s { background : %s }\n",clstr,hcstr);

      seg = seg->rseg;
      n++;
    }

  return 0;
}

