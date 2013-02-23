/*
  cptsvg.c

  (c) J.J.Green 2001,2005
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cptio.h"
#include "svgwrite.h"

#include "cptsvg.h"

static int cptsvg_convert(cpt_t*,svg_t*,cptsvg_opt_t);

extern int cptsvg(char* infile, char* outfile, cptsvg_opt_t opt)
{
    cpt_t *cpt;
    svg_t *svg;
    int err=0;

    if ((cpt = cpt_new()) != NULL)
      {
 	if (cpt_read(infile,cpt,0) == 0)
	  {
	    if ((svg = svg_new()) != NULL)
	      {
		if (cptsvg_convert(cpt,svg,opt) == 0)
		  {
		    if (svg_write(outfile, svg, &(opt.preview)) == 0)
		      {
			/* success */
		      }
		    else
		      {
			fprintf(stderr,"error writing sgv struct\n");
			err = 1;
		      }
		  }
		else
		  {
		    fprintf(stderr,"failed to convert cpt to svg\n");
		    err = 1;
		  }

		svg_destroy(svg);
	      }
	    else
	      {
		fprintf(stderr,"failed to allocate svg\n");
		err = 1;
	      }
	  }
	else
	  {
	    fprintf(stderr,"failed to load cpt from %s\n",(infile ? infile : "<stdin>"));
	    err = 1;
	  }

	cpt_destroy(cpt);    
      }	
    else
      {
	fprintf(stderr,"failed to create cpt struct\n");
	err = 1;
      }

    if (err)
      fprintf(stderr,"failed to write svg to %s\n",(outfile ? outfile : "<stdout>"));

    return err;
}

static int cptsvg_convert(cpt_t* cpt,svg_t* svg,cptsvg_opt_t opt)
{
  cpt_seg_t *seg;
  svg_stop_t lstop,rstop;
  double min,max;
  int n,m=0;

  /* check we have at least one cpt segment */

  if (cpt->segment == NULL)
    {
      fprintf(stderr,"cpt has no segments\n");
      return 1;
    }

  /* copy the name */

  if (snprintf((char*)svg->name,
	       SVG_NAME_LEN,
	       "%s",cpt->name) >= SVG_NAME_LEN)
    {
      fprintf(stderr,"truncated svg name!\n");
    }

  /* find the min/max of the cpt range */

  min = cpt->segment->lsmp.val;

  max = cpt->segment->rsmp.val;
  for (seg=cpt->segment ; seg ; seg=seg->rseg)
    max = seg->rsmp.val;
  
  /* get the colour model */

  switch (cpt->model)
    {
    case model_rgb: 
      break;
    case model_hsv: 
      break;
    default:
      fprintf(stderr,"unknown colour model\n");
      return 1;
    }

  /* convert cpt segments to svg stops*/

  for (n=0,seg=cpt->segment ; seg ; seg=seg->rseg)
    {
      cpt_sample_t lsmp,rsmp;
      rgb_t rcol,lcol;
      filltype_t type;

      lsmp = seg->lsmp;
      rsmp = seg->rsmp;

      if (lsmp.fill.type != rsmp.fill.type)
        {
          fprintf(stderr,"sorry, can't convert mixed fill types\n");
          return 1;
        }
            
      type = lsmp.fill.type;

      switch (type)
        {
        case fill_colour:
	  
          switch (cpt->model)
            {
            case model_rgb:
              lcol = lsmp.fill.u.colour.rgb;
              rcol = rsmp.fill.u.colour.rgb;
              break;
            case model_hsv:
	      /* fixme */
	      fprintf(stderr,"conversion of hsv not yet implemeted\n");
              return 1;
            default:
              return 1;
            }
          break;
	  
        case fill_grey:
        case fill_hatch:
        case fill_file:
        case fill_empty:
	  /* fixme */
	  fprintf(stderr,"fill type not implemeted yet\n");
          return 1;

        default:
          fprintf(stderr,"strange fill type\n");
          return 1;
        }

      /* always insert the left colour */

      lstop.value   = 100*(lsmp.val-min)/(max-min);
      lstop.colour  = lcol;
      lstop.opacity = 1.0; 

      if (svg_append(lstop,svg) == 0) m++;
      else 
	{
	  fprintf(stderr,"error adding stop for segment %i left\n",n);
	  return 1;
	}

      /*
	if there is a segment to the right, and if its left
	segment is the same colour at the our right segment
	then dont insert it. Otherwise do.
      */

      if ( ! ((seg->rseg) && fill_eq(rsmp.fill, seg->rseg->lsmp.fill)))
	{
	  rstop.value   = 100*(rsmp.val-min)/(max-min);
	  rstop.colour  = rcol;
	  rstop.opacity = 1.0; 

	  if (svg_append(rstop,svg) == 0) m++;
	  else
	    {
	      fprintf(stderr,"error adding stop for segment %i right\n",n);
	      return 1;
	    }
	}

      n++;
    }

  if (opt.verbose)
    printf("converted %i segments to %i stops\n",n,m);

  return 0;
}


