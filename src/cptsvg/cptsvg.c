/*
  cptgimp.c

  (c) J.J.Green 2001,2005
  $Id: cptsvg.c,v 1.1 2005/05/30 23:16:38 jjg Exp jjg $
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "cpt.h"
#include "cptio.h"
#include "version.h"

#include "cptsvg.h"

#define ENCODING "utf-8"

static int cptsvg_convert(cpt_t*,xmlTextWriter*,cptsvg_opt_t);

extern int cptsvg(char* infile,char* outfile,cptsvg_opt_t opt)
{
    cpt_t* cpt;
    int err=0;

    LIBXML_TEST_VERSION

    if ((cpt = cpt_new()) != NULL)
      {
 	if (cpt_read(infile,cpt,0) == 0)
	  {
	    xmlBuffer* buffer;
	    
	    if ((buffer = xmlBufferCreate()) != NULL)
	      {
		xmlTextWriter* writer;

		if ((writer = xmlNewTextWriterMemory(buffer,0)) != NULL) 
		  {
		    /*
		      we free the writer at the start of each of
		      these branches since this must be done before
		      using the buffer
		    */

		    if (cptsvg_convert(cpt,writer,opt) == 0)
		      {
			const char *content = buffer->content;

			xmlFreeTextWriter(writer);

			if (outfile)
			  {
			    FILE* fp;
			    
			    if ((fp = fopen(outfile, "w")) != NULL) 
			      {
				fprintf(fp,"%s",content);
				
				if (fclose(fp) != 0)
				  {
				    fprintf(stderr,"error closing file %s\n",outfile);
				    err = 1;
				  }
			      }
			    else
			      {
				fprintf(stderr,"error opening file %s\n",outfile);
				err = 1;
			      }
			  }
			else
			  fprintf(stdout,"%s",content);
		      }
		    else
		      {
			xmlFreeTextWriter(writer);

			fprintf(stderr,"failed to convert cpt\n");
			err = 1;
		      }
		  }
		else
		  {
		    fprintf(stderr,"error creating the xml writer\n");
		    err = 1;
		  }
	      }		
	    else
	      {
		fprintf(stderr,"error creating xml writer buffer\n");
		err = 1;
	      }

	    xmlBufferFree(buffer);
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
      fprintf(stderr,"failed to create %s\n",(outfile ? outfile : "<stdout>"));

    return err;
}

static int cptsvg_convert_stop(xmlTextWriter*,double,rgb_t,double);
static const char* timestring(void);

static int cptsvg_convert(cpt_t* cpt,xmlTextWriter* writer,cptsvg_opt_t opt)
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

  if (
      xmlTextWriterWriteAttribute(writer,BAD_CAST "version",BAD_CAST "1.1") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "xmlns",BAD_CAST  "http://www.w3.org/2000/svg") < 0
      )
    {
      fprintf(stderr,"error from svg attribute\n");
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
	      fprintf(stderr,"conversion of hsv not yet implemeted\n");
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

      cptsvg_convert_stop(writer,100*(lsmp.val-min)/(max-min),lcol,1.0);
      cptsvg_convert_stop(writer,100*(rsmp.val-min)/(max-min),rcol,1.0);

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

  if (xmlTextWriterStartElement(writer,BAD_CAST "creator") < 0)
    {
      fprintf(stderr,"error from open creator\n");
      return 1;
    }

  if (
      xmlTextWriterWriteAttribute(writer,BAD_CAST "name",BAD_CAST "cptsvg") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "version",BAD_CAST VERSION) < 0
      )
    {

      fprintf(stderr,"error from creator attribute\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close creator\n");
      return 1;
    }

  if (xmlTextWriterStartElement(writer,BAD_CAST "created") < 0)
    {
      fprintf(stderr,"error from open metadata\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer,BAD_CAST "date",BAD_CAST timestring()) < 0)
    {
      fprintf(stderr,"error from created attribute\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close created\n");
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

static const char* timestring(void)
{
  time_t  tm;
  char* tmstr;
  static char ts[25]; 

  time(&tm);
  tmstr = ctime(&tm);

  sprintf(ts,"%.24s",tmstr);

  return ts;
}

#define BUFSZ 128

static int cptsvg_convert_stop(xmlTextWriter* writer,double val,rgb_t col,double opacity)
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


