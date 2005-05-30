/*
  cptgimp.c

  (c) J.J.Green 2001,2005
  $Id: cptgimp.c,v 1.6 2004/03/22 01:05:44 jjg Exp $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "cpt.h"
#include "cptio.h"
#include "version.h"

#include "cptsvg.h"

#define ENCODING "utf-8"

static int cptsvg_write(cpt_t*,xmlTextWriter*,cptsvg_opt_t);

extern int cptsvg(char* infile,char* outfile,cptsvg_opt_t opt)
{
    cpt_t* cpt;
    xmlTextWriter* writer;

    LIBXML_TEST_VERSION

    /* load the cpt */
    
    if ((cpt = cpt_new()) == NULL)
      {
	fprintf(stderr,"failed to create cpt struct\n");
	return 1;
      }

    if (cpt_read(infile,cpt,0) != 0)
      {
	fprintf(stderr,"failed to load cpt from ");
	if (infile)
	  {
	    fprintf(stderr,"%s\n",infile);
	    free(infile);
	  }
	else
	  fprintf(stderr,"<stdin>\n");
	
	return 1;
      }
    
    /* load the writer */

    if ((writer = xmlNewTextWriterFilename(outfile,0)) == NULL) 
      {
	fprintf(stderr,"error creating the xml writer\n");
	return 1;
      }

    /* write cpt */

    if (cptsvg_write(cpt,writer,opt) != 0)
      {
	fprintf(stderr,"failed to convert cpt\n");
	return 1;
      }

    /* tidy */

    xmlFreeTextWriter(writer);    
    cpt_destroy(cpt);    

    return 0;
}

static int cptsvg_write_stop(xmlTextWriter*,double,rgb_t,double);

static int cptsvg_write(cpt_t* cpt,xmlTextWriter* writer,cptsvg_opt_t opt)
{
  cpt_seg_t* seg;
  double min,max;
  int n;

  /* check we have at least one cpt segment */

  if (cpt->segment == NULL)
    {
      fprintf(stderr,"cpt has no segments\n");
      return 1;
    }

  /* find the min/max of the cpt range */

  min = cpt->segment->lsmp.val;

  max = cpt->segment->rsmp.val;
  for (seg=cpt->segment ; seg ; seg=seg->rseg)
    max = seg->rsmp.val;
  
  /* get the colour model */

  switch (cpt->model)
    {
    case rgb: 
      break;
    case hsv: 
      break;
    default:
      fprintf(stderr,"unknown colour model\n");
      return 1;
    }

  /* write the xml */

  if (xmlTextWriterStartDocument(writer,NULL,ENCODING,NULL) < 0)
    {
      fprintf(stderr,"error from start document\n");
      return 1;
    }

  if (xmlTextWriterStartElement(writer,BAD_CAST "svg") < 0)
    {
      fprintf(stderr,"error from open svg\n");
      return 1;
    }

  if (xmlTextWriterStartElement(writer,BAD_CAST "linearGradient") < 0)
    {
      fprintf(stderr,"error from open linearGradient\n");
      return 1;
    }

  if (
      xmlTextWriterWriteAttribute(writer,BAD_CAST "id",BAD_CAST cpt->name) < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "gradientUnits",BAD_CAST "objectBoundingBox") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "spreadMethod",BAD_CAST "pad") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "x1",BAD_CAST "0%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "x2",BAD_CAST "100%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "y1",BAD_CAST "0%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "y2",BAD_CAST "100%") < 0
      )
    {
      fprintf(stderr,"error from linearGradient attribute\n");
      return 1;
    }

  /*
    currently we write the left and right fill values as
    stops, but this means redundancy in the conversion of
    continuous palettes

    fixme, by creating an array of stops and then printing
    just those
  */

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
        case colour:
	  
          switch (cpt->model)
            {
            case rgb:
              lcol = lsmp.fill.u.colour.rgb;
              rcol = rsmp.fill.u.colour.rgb;
              break;
            case hsv:
	      /* fixme */
	      fprintf(stderr,"hsv not implemeted yet\n");
              return 1;
            default:
              return 1;
            }
          break;
	  
        case grey:
        case hatch:
        case file:
        case empty:
	  /* fixme */
	  fprintf(stderr,"fill type not implemeted yet\n");
          return 1;

        default:
          fprintf(stderr,"strange fill type\n");
          return 1;
        }

      cptsvg_write_stop(writer,100*(lsmp.val-min)/(max-min),lcol,1.0);
      cptsvg_write_stop(writer,100*(rsmp.val-min)/(max-min),rcol,1.0);

      n++;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close linearGradient\n");
      return 1;
    }

  if (xmlTextWriterStartElement(writer,BAD_CAST "metadata") < 0)
    {
      fprintf(stderr,"error from open metadata\n");
      return 1;
    }

  if (
      xmlTextWriterWriteAttribute(writer,BAD_CAST "creator",BAD_CAST "cptsvg") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "version",BAD_CAST VERSION) < 0
      )
    {

      fprintf(stderr,"error from open metadata\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close metadata\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close svg\n");
      return 1;
    }

  if (xmlTextWriterEndDocument(writer) < 0)
    {
      fprintf(stderr,"error from end document\n");
      return 1;
    }

  if (opt.verbose)
    printf("converted %i segments\n",n);

  return 0;
}

#define BUFSZ 128

static int cptsvg_write_stop(xmlTextWriter* writer,double val,rgb_t col,double opacity)
{
  char obuf[BUFSZ],scbuf[BUFSZ],sobuf[BUFSZ];

  snprintf(obuf, BUFSZ,"%.2f%%",val);
  snprintf(scbuf,BUFSZ,"rgb(%i,%i,%i)",col.red, col.green, col.blue);
  snprintf(sobuf,BUFSZ,"%.4f",opacity);

  if (xmlTextWriterStartElement(writer,BAD_CAST "stop") < 0)
    {
      fprintf(stderr,"error from open stop\n");
      return 1;
    }
  
  if (
      xmlTextWriterWriteAttribute(writer,BAD_CAST "offset",BAD_CAST obuf) < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "stop-color",BAD_CAST scbuf) < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "stop-opacity",BAD_CAST sobuf) < 0
      )
    {
      fprintf(stderr,"error from stop attribute\n");
      return 1;
    }
  
  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close stop\n");
      return 1;
    }
  
  return 0;
}  


