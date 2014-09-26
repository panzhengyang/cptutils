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
#include "btrace.h"

#define ENCODING "utf-8"
#define BUFSZ 128

static int svg_write_mem(xmlTextWriter*,
			 size_t,
			 const svg_t**, 
			 const svg_preview_t*);

extern int svg_write(const char *file,
		     size_t n,
		     const svg_t **svg, 
		     const svg_preview_t *preview)
{
  xmlBuffer* buffer;
  int err = 0;
	    
  if ((buffer = xmlBufferCreate()) != NULL)
    {
      xmlTextWriter* writer;
      
      if ((writer = xmlNewTextWriterMemory(buffer, 0)) != NULL) 
	{
	  /*
	    we free the writer at the start of each of
	    these branches since this must be done before
	    using the buffer
	  */
	  
	  if (svg_write_mem(writer, n, svg, preview) == 0)
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
		      fprintf(fp, "%s", buffer->content);
		      
		      if (fclose(fp) != 0)
			{
			  btrace("error closing file %s", file);
			  err = 1;
			}
		    }
		  else
		    {
		      btrace("error opening file %s", file);
		      err = 1;
		    }
		}
	      else
		fprintf(stdout, "%s", buffer->content);
	    }
	  else
	    {
	      xmlFreeTextWriter(writer);
	      
	      btrace("failed memory write");
	      err = 1;
	    }
	}
      else
	{
	  btrace("error creating the xml writer");
	  err = 1;
	}
    }		
  else
    {
      btrace("error creating xml writer buffer");
      err = 1;
    }
  
  xmlBufferFree(buffer);

  return err;
}

/* handle error messages setting attributes */

static int svg_attribute(xmlTextWriter *writer, 
			 const char *name, 
			 const char *value, 
			 const char *element)
{    
  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST name, 
				  BAD_CAST value) < 0)
    {
      btrace("error setting %s %s attribute", element, name);
      return 1;
    }
   
  return 0;
}

static int svg_write_lineargradient(xmlTextWriter*, const svg_t*);
static int svg_write_metadata(xmlTextWriter*);

static const char* timestring(void);

static int svg_write_mem(xmlTextWriter *writer,
			 size_t n,
			 const svg_t **svg, 
			 const svg_preview_t *preview)
{
  size_t i;

  /* check at least one gradient */

  if (n < 1)
    {
      btrace("no gradients to write");
      return 1;
    }

  /* no preview with multi-gradients (yet) */

  if ((n > 1) && preview->use)
    {
      btrace("no previews with multi-gradient output (yet)");
      return 1;
    }

  /* check that all svgs have at least one segment */

  for (i=0 ; i<n ; i++)
    {
      if ( svg[i]->nodes == NULL )
	{
	  btrace("svg %zu has no segments", i);
	  return 1;
	}
    }

  /* start xml */

  if ( xmlTextWriterStartDocument(writer, NULL, ENCODING, NULL) < 0 )
    {
      btrace("error from start document");
      return 1;
    }

  /* svg */

  if ( xmlTextWriterStartElement(writer, BAD_CAST "svg") < 0 )
    {
      btrace("error from open svg");
      return 1;
    }

  if (svg_attribute(writer, "version", "1.1", "svg") != 0)
    return 1;

  if (svg_attribute(writer, "xmlns", "http://www.w3.org/2000/svg", "svg") != 0)
    return 1;

  if ( preview->use )
    {
      char str[BUFSZ];
      
      if (snprintf(str, BUFSZ, "%zupx", preview->width) >= BUFSZ)
	return 1;
      
      if (svg_attribute(writer, "width", str, "svg") != 0)
	return 1;
      
      if (snprintf(str, BUFSZ, "%zupx", preview->height) >= BUFSZ)
	return 1;
      
      if (svg_attribute(writer, "height", str, "svg") != 0)
	return 1;

      if (snprintf(str, BUFSZ, "0 0 %zu %zu", 
		   preview->width, 
		   preview->height) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "viewBox", str, "svg") != 0)
	return 1;
    }

  if ( preview->use )
    {
      char str[BUFSZ];

      if ( xmlTextWriterStartElement(writer, BAD_CAST "g") < 0 )
	{
	  btrace("error from open g");
	  return 1;
	}

      if ( xmlTextWriterStartElement(writer, BAD_CAST "defs") < 0 )
	{
	  btrace("error from open defs");
	  return 1;
	}

      for (i=0 ; i<n ; i++)
	{
	  if ( svg_write_lineargradient(writer, svg[i]) != 0 )
	    return 1;
	}

      if ( xmlTextWriterEndElement(writer) < 0 )
	{
	  btrace("error from close defs");
	  return 1;
	}

      /* preview rectangle */

      if ( xmlTextWriterStartElement(writer, BAD_CAST "rect") < 0 )
	{
	  btrace("error from open rect");
	  return 1;
	}

      if (snprintf(str, BUFSZ, "url(#%s)", svg[0]->name) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "fill", str, "rect") != 0)
	return 1;

      if (snprintf(str, BUFSZ, "%zu", preview->border) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "x", str, "rect") != 0)
	return 1;

      if (svg_attribute(writer, "y", str, "rect") != 0)
	return 1;
      
      if (snprintf(str, BUFSZ, "%zu", 
		   preview->width - 2 * preview->border) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "width", str, "rect") != 0)
	return 1;
      
      if (snprintf(str, BUFSZ, "%zu", 
		   preview->height - 2 * preview->border) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "height", str, "rect") != 0)
	return 1;

      if (svg_attribute(writer, "stroke", "black", "rect") != 0)
	return 1;

      if (snprintf(str, BUFSZ, "%zu", preview->stroke) >= BUFSZ)
	return 1;
  
      if (svg_attribute(writer, "stroke-width", str, "rect") != 0)
	return 1;

      if ( xmlTextWriterEndElement(writer) < 0 )
	{
	  btrace("error from close rect");
	  return 1;
	}

      if ( xmlTextWriterEndElement(writer) < 0 )
	{
	  btrace("error from close g");
	  return 1;
	}

      if ( svg_write_metadata(writer) != 0 )
	return 1;
    }
  else
    {
      for (i=0 ; i<n ; i++)
	{
	  if ( svg_write_lineargradient(writer, svg[i]) != 0 )
	    return 1;
	}

      if ( svg_write_metadata(writer) != 0 )
	return 1;
    }

  if ( xmlTextWriterEndElement(writer) < 0 )
    {
      btrace("error from close svg");
      return 1;
    }

  if ( xmlTextWriterEndDocument(writer) < 0 )
    {
      btrace("error from end document");
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

  sprintf(ts, "%.24s", tmstr);

  return ts;
}

static int svg_write_metadata(xmlTextWriter *writer)
{
  if (xmlTextWriterStartElement(writer, BAD_CAST "metadata") < 0)
    {
      btrace("error from open metadata");
      return 1;
    }

  if (xmlTextWriterStartElement(writer, BAD_CAST "creator") < 0)
    {
      btrace("error from open creator");
      return 1;
    }

  if (svg_attribute(writer, "name", "cptutils", "creator") != 0)
    return 1;

  if (svg_attribute(writer, "version", VERSION, "creator") != 0)
    return 1;

  if (xmlTextWriterEndElement(writer) < 0)
    {
      btrace("error from close creator");
      return 1;
    }

  if (xmlTextWriterStartElement(writer, BAD_CAST "created") < 0)
    {
      btrace("error from open created");
      return 1;
    }

  if (svg_attribute(writer, "date", timestring(), "created") != 0)
    return 1;

  if (xmlTextWriterEndElement(writer) < 0)
    {
      btrace("error from close created");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      btrace("error from close metadata");
      return 1;
    }

  return 0;
}

static int svg_write_stop(xmlTextWriter*, svg_stop_t);

static int svg_write_lineargradient(xmlTextWriter *writer, const svg_t *svg)
{
  int err = 0;

  if (xmlTextWriterStartElement(writer, BAD_CAST "linearGradient") < 0)
    {
      btrace("error from open linearGradient");
      return 1;
    }

  /* atypical attribute, svg->name is unsigned char* */
  
  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "id", 
				  svg->name) < 0)
    {
      btrace("error writing linearGradient id attribute");
      return 1;
    }

  err += (svg_attribute(writer, "gradientUnits", "objectBoundingBox", 
			"linearGradient") != 0);
  err += (svg_attribute(writer, "spreadMethod", "pad", 
			"linearGradient") != 0);
  err += (svg_attribute(writer, "x1", "0%", "linearGradient") != 0);
  err += (svg_attribute(writer, "x2", "100%", "linearGradient") != 0);
  err += (svg_attribute(writer, "y1", "0%", "linearGradient") != 0);
  err += (svg_attribute(writer, "y2", "0%", "linearGradient") != 0);

  if (err) return 1;

  svg_node_t *node;

  for (node=svg->nodes ; node ; node=node->r)
    {
      svg_write_stop(writer, node->stop);
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      btrace("error from close linearGradient");
      return 1;
    }

  return 0;
}

static int svg_write_stop(xmlTextWriter* writer, svg_stop_t stop)
{
  char obuf[BUFSZ], scbuf[BUFSZ], sobuf[BUFSZ];
  rgb_t col;
  double value, opacity;

  col     = stop.colour;
  value   = stop.value;
  opacity = stop.opacity;

  snprintf(obuf, BUFSZ, "%.2f%%", value);
  snprintf(scbuf, BUFSZ, "rgb(%i, %i, %i)", col.red, col.green, col.blue);
  snprintf(sobuf, BUFSZ, "%.4f", opacity);

  if (xmlTextWriterStartElement(writer, BAD_CAST "stop") < 0)
    {
      btrace("error from open stop");
      return 1;
    }

  int err = 0;

  err += (svg_attribute(writer, "offset", obuf, "stop") != 0);
  err += (svg_attribute(writer, "stop-color", scbuf, "stop") != 0);
  err += (svg_attribute(writer, "stop-opacity", sobuf, "stop") != 0);

  if (err) return 1;
  
  if (xmlTextWriterEndElement(writer) < 0)
    {
      btrace("error from close stop");
      return 1;
    }
  
  return 0;
}  


