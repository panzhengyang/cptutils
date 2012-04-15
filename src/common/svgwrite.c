/*
  svgwrite.c

  reads & writes svg gradient files using libxml2

  J.J. Green 2005
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <stdio.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "svgwrite.h"

#define ENCODING "utf-8"

static int svg_write_mem(xmlTextWriter*, const svg_t*, const svg_preview_t*);

extern int svg_write(const char *file, 
		     const svg_t *svg, 
		     const svg_preview_t *preview)
{
  xmlBuffer* buffer;
  int err = 0;
	    
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
	  
	  if (svg_write_mem(writer, svg, preview) == 0)
	    {
	      /*
		don't take a 

		  const char *buffer = buffer->content

		to use in place of buffer->content here,
		that trips some wierd behaviour in gcc
		and leads to corruped xml!
	      */

	      xmlFreeTextWriter(writer);
	      
	      if (file)
		{
		  FILE* fp;
		  
		  if ((fp = fopen(file, "w")) != NULL) 
		    {
		      fprintf(fp,"%s",buffer->content);
		      
		      if (fclose(fp) != 0)
			{
			  fprintf(stderr,"error closing file %s\n",file);
			  err = 1;
			}
		    }
		  else
		    {
		      fprintf(stderr,"error opening file %s\n",file);
		      err = 1;
		    }
		}
	      else
		fprintf(stdout,"%s",buffer->content);
	    }
	  else
	    {
	      xmlFreeTextWriter(writer);
	      
	      fprintf(stderr,"failed memory write\n");
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

  return err;
}

static int svg_write_lineargradient(xmlTextWriter*, const svg_t*);
static int svg_write_metadata(xmlTextWriter*, const svg_t*);

static const char* timestring(void);

static int svg_write_mem(xmlTextWriter *writer, 
			 const svg_t *svg, 
			 const svg_preview_t *preview)
{
  /* check we have at least one segment */

  if ( svg->nodes == NULL )
    {
      fprintf(stderr,"svg has no segments\n");
      return 1;
    }

  /* write the xml */

  if ( xmlTextWriterStartDocument(writer,NULL,ENCODING,NULL) < 0 )
    {
      fprintf(stderr,"error from start document\n");
      return 1;
    }

  if ( xmlTextWriterStartElement(writer,BAD_CAST "svg") < 0 )
    {
      fprintf(stderr,"error from open svg\n");
      return 1;
    }

  if (
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "version",
				  BAD_CAST "1.1") < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "xmlns",
				  BAD_CAST  "http://www.w3.org/2000/svg") < 0
      )
    {
      fprintf(stderr,"error from svg attribute\n");
      return 1;
    }

  if ( preview->use )
    {
      if ( xmlTextWriterStartElement(writer,BAD_CAST "g") < 0 )
	{
	  fprintf(stderr,"error from open g\n");
	  return 1;
	}

      if ( xmlTextWriterStartElement(writer,BAD_CAST "defs") < 0 )
	{
	  fprintf(stderr,"error from open defs\n");
	  return 1;
	}

      if ( svg_write_lineargradient(writer, svg) != 0 )
	return 1;

      if ( xmlTextWriterEndElement(writer) < 0 )
	{
	  fprintf(stderr,"error from close defs\n");
	  return 1;
	}

      // rectangle

      if ( xmlTextWriterEndElement(writer) < 0 )
	{
	  fprintf(stderr,"error from close g\n");
	  return 1;
	}

      if ( svg_write_metadata(writer, svg) != 0 )
	return 1;
    }
  else
    {
      if ( svg_write_lineargradient(writer, svg) != 0 )
	return 1;

      if ( svg_write_metadata(writer, svg) != 0 )
	return 1;
    }

  if ( xmlTextWriterEndElement(writer) < 0 )
    {
      fprintf(stderr,"error from close svg\n");
      return 1;
    }

  if ( xmlTextWriterEndDocument(writer) < 0 )
    {
      fprintf(stderr,"error from end document\n");
      return 1;
    }

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

static int svg_write_metadata(xmlTextWriter *writer, const svg_t *svg)
{
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
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "name",
				  BAD_CAST "cptutils") < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "version",
				  BAD_CAST VERSION) < 0
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
      fprintf(stderr,"error from open created\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "date",
				  BAD_CAST timestring()) < 0)
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

  return 0;
}

static int svg_write_stop(xmlTextWriter*,svg_stop_t);

static int svg_write_lineargradient(xmlTextWriter *writer, const svg_t *svg)
{

  if (xmlTextWriterStartElement(writer,BAD_CAST "linearGradient") < 0)
    {
      fprintf(stderr,"error from open linearGradient\n");
      return 1;
    }

  if (
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "id",
				  BAD_CAST svg->name) < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "gradientUnits",
				  BAD_CAST "objectBoundingBox") < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "spreadMethod",
				  BAD_CAST "pad") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "x1",BAD_CAST "0%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "x2",BAD_CAST "100%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "y1",BAD_CAST "0%") < 0 ||
      xmlTextWriterWriteAttribute(writer,BAD_CAST "y2",BAD_CAST "0%") < 0
      )
    {
      fprintf(stderr,"error from linearGradient attribute\n");
      return 1;
    }

  svg_node_t *node;

  for (node=svg->nodes ; node ; node=node->r)
    {
      svg_write_stop(writer,node->stop);
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close linearGradient\n");
      return 1;
    }

  return 0;
}

#define BUFSZ 128

static int svg_write_stop(xmlTextWriter* writer, svg_stop_t stop)
{
  char obuf[BUFSZ],scbuf[BUFSZ],sobuf[BUFSZ];
  rgb_t col;
  double value,opacity;

  col     = stop.colour;
  value   = stop.value;
  opacity = stop.opacity;

  snprintf(obuf, BUFSZ,"%.2f%%",value);
  snprintf(scbuf,BUFSZ,"rgb(%i,%i,%i)",col.red, col.green, col.blue);
  snprintf(sobuf,BUFSZ,"%.4f",opacity);

  if (xmlTextWriterStartElement(writer,BAD_CAST "stop") < 0)
    {
      fprintf(stderr,"error from open stop\n");
      return 1;
    }
  
  if (
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "offset",
				  BAD_CAST obuf) < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "stop-color",
				  BAD_CAST scbuf) < 0 ||
      xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "stop-opacity",
				  BAD_CAST sobuf) < 0
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


