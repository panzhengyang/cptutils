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
#define BUFSZ 128

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
      fprintf(stderr,"error setting %s %s attribute\n",element,name);
      return 1;
    }
   
  return 0;
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

  /* start xml */

  if ( xmlTextWriterStartDocument(writer,NULL,ENCODING,NULL) < 0 )
    {
      fprintf(stderr,"error from start document\n");
      return 1;
    }

  /* svg */

  if ( xmlTextWriterStartElement(writer,BAD_CAST "svg") < 0 )
    {
      fprintf(stderr,"error from open svg\n");
      return 1;
    }

  if (svg_attribute(writer,"version","1.1","svg") != 0)
    return 1;

  if (svg_attribute(writer,"xmlns","http://www.w3.org/2000/svg","svg") != 0)
    return 1;

  if ( preview->use )
    {
      char str[BUFSZ];
      
      if (snprintf(str,BUFSZ,"%zupx",preview->width) >= BUFSZ)
	return 1;
      
      if (svg_attribute(writer,"width",str,"svg") != 0)
	return 1;
      
      if (snprintf(str,BUFSZ,"%zupx",preview->height) >= BUFSZ)
	return 1;
      
      if (svg_attribute(writer,"height",str,"svg") != 0)
	return 1;

      if (snprintf(str,BUFSZ,"0 0 %zu %zu",
		   preview->width,
		   preview->height) >= BUFSZ)
	return 1;

      if (svg_attribute(writer,"viewBox",str,"svg") != 0)
	return 1;
    }

  if ( preview->use )
    {
      char str[BUFSZ];

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

      /* preview rectangle */

      if ( xmlTextWriterStartElement(writer,BAD_CAST "rect") < 0 )
	{
	  fprintf(stderr,"error from open rect\n");
	  return 1;
	}

      if (snprintf(str,BUFSZ,"url(#%s)",svg->name) >= BUFSZ)
	return 1;

      if (svg_attribute(writer,"fill",str,"rect") != 0)
	return 1;

      if (snprintf(str,BUFSZ,"%zu",preview->border) >= BUFSZ)
	return 1;

      if (svg_attribute(writer,"x",str,"rect") != 0)
	return 1;

      if (svg_attribute(writer,"y",str,"rect") != 0)
	return 1;
      
      if (snprintf(str,BUFSZ,"%zu",
		   preview->width - 2 * preview->border) >= BUFSZ)
	return 1;

      if (svg_attribute(writer, "width", str, "rect") != 0)
	return 1;
      
      if (snprintf(str,BUFSZ,"%zu",
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
	  fprintf(stderr,"error from close rect\n");
	  return 1;
	}

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

  if (svg_attribute(writer,"name","cptutils","creator") != 0)
    return 1;

  if (svg_attribute(writer,"version",VERSION,"creator") != 0)
    return 1;

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

  if (svg_attribute(writer,"date",timestring(),"created") != 0)
    return 1;

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
  int err = 0;

  if (xmlTextWriterStartElement(writer,BAD_CAST "linearGradient") < 0)
    {
      fprintf(stderr,"error from open linearGradient\n");
      return 1;
    }

  /* atypical attribute, svg->name is unsigned char* */
  
  if (xmlTextWriterWriteAttribute(writer,
				  BAD_CAST "id",
				  svg->name) < 0)
    {
      fprintf(stderr,"error writing linearGradient id attribute\n");
      return 1;
    }

  err += (svg_attribute(writer,"gradientUnits","objectBoundingBox",
			"linearGradient") != 0);
  err += (svg_attribute(writer,"spreadMethod","pad",
			"linearGradient") != 0);
  err += (svg_attribute(writer,"x1","0%","linearGradient") != 0);
  err += (svg_attribute(writer,"x2","100%","linearGradient") != 0);
  err += (svg_attribute(writer,"y1","0%","linearGradient") != 0);
  err += (svg_attribute(writer,"y2","0%","linearGradient") != 0);

  if (err) return 1;

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

  int err = 0;

  err += (svg_attribute(writer,"offset",obuf,"stop") != 0);
  err += (svg_attribute(writer,"stop-color",scbuf,"stop") != 0);
  err += (svg_attribute(writer,"stop-opacity",sobuf,"stop") != 0);

  if (err) return 1;
  
  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr,"error from close stop\n");
      return 1;
    }
  
  return 0;
}  


