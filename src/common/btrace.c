/*
  btrace.c

  Copyright (c) J.J. Green 2014
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_JANSSON_H
#include <jansson.h>
#endif

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "btrace.h"

#define BUFSZ 512

typedef struct btrace_line_t btrace_line_t;

struct btrace_line_t
{
  int            line;
  const char    *file;
  char          *message;
  btrace_line_t *next;
};

typedef struct
{
  const char *program;
  btrace_line_t *lines;
} btrace_t;

static btrace_t btrace_global = 
  {
    .program = NULL,
    .lines   = NULL
  };

static const char* date_string(void)
{
  time_t t = time(NULL);
  struct tm* bdt = gmtime(&t); 
  static char buffer[32];

  snprintf(buffer, 32, 
	   "%04d-%02d-%02dT%02d:%02d",
	   bdt->tm_year + 1900,
	   bdt->tm_mon + 1,
	   bdt->tm_mday,
	   bdt->tm_hour,
	   bdt->tm_min);

  return buffer;
}

/* string to format */

extern int btrace_format(const char* name)
{
  if (name == NULL) 
    return BTRACE_NONE;
  else if (strcmp(name, "plain") == 0)
    return BTRACE_PLAIN;
  else if (strcmp(name, "xml") == 0)
    return BTRACE_XML;
  else if (strcmp(name, "json") == 0)
    return BTRACE_JSON;
    
  return BTRACE_ERROR;
}

/* enable/disable */

static void enable(const char *program, btrace_t *bt)
{
  bt->program = program;
}

extern void btrace_enable(const char *program)
{
  enable(program, &btrace_global); 
}

static void disable(btrace_t *bt)
{
  bt->program = NULL;
}

extern void btrace_disable(void)
{
  disable(&btrace_global);
}

static bool is_enabled(btrace_t *bt)
{
  return bt->program != NULL;
}

extern bool btrace_is_enabled(void)
{
  return is_enabled(&btrace_global); 
}

/* testing nonempty */

static bool is_empty(btrace_t* bt)
{
  return bt->lines == NULL;
}

extern bool btrace_is_empty(void)
{
  return is_empty(&btrace_global);
}

/* free lines */

static void line_free(btrace_line_t *btl)
{
  free(btl->message);
  free(btl);
}

static void lines_free(btrace_line_t *btl)
{
  if (btl)
    {
      lines_free(btl->next);
      line_free(btl);
    }
}

/* reset the btrace */

static void reset(btrace_t *bt)
{
  lines_free(bt->lines);
  bt->lines = NULL;
}

extern void btrace_reset(void)
{
  reset(&btrace_global);
}

/* count lines */

static size_t count_lines(btrace_line_t *btl)
{
  return btl == NULL ? 0 : count_lines(btl->next) + 1;
}

static size_t count(btrace_t *bt)
{
  return count_lines(bt->lines);
}

extern size_t btrace_count(void)
{
  return count(&(btrace_global));
}

/* adding */

static btrace_line_t* line_new(const char* file, int line, char *message)
{
  btrace_line_t *btl;

  if ((btl = malloc(sizeof(btrace_line_t))) != NULL)
    {
      if ((btl->message = strdup(message)) != NULL)
	{
	  btl->file = file;
	  btl->line = line;

	  return btl;
	}
      free(btl);
    }

  return NULL;
}

static void append(btrace_t *bt, const char* file, int line, char *message)
{
  btrace_line_t *btl;

  if ((btl = line_new(file, line, message)) == NULL)
    return;

  btl->next = bt->lines;
  bt->lines = btl;
}

extern void btrace_add(const char* file, int line, const char* format, ...)
{
  char buffer[512];
  va_list args;
  
  va_start(args, format);
  vsnprintf(buffer, BUFSZ, format, args);
  va_end(args);

  append(&btrace_global, file, line, buffer);
}

/* print plain format */

static int line_print_plain(FILE *stream, btrace_line_t *btl)
{
  fprintf(stream, "%s (%s, %i)\n", btl->message, btl->file, btl->line);

  return 0;
}

static int lines_print_plain(FILE *stream, btrace_line_t *btl)
{
  if (btl)
    {
      return 
	lines_print_plain(stream, btl->next) +
	line_print_plain(stream, btl);
    }

  return 0;
}

static int print_plain(FILE *stream, btrace_t *bt)
{
  return lines_print_plain(stream, bt->lines);
}

/* print XML format */

static int line_print_xml(xmlTextWriter* writer, btrace_line_t *btl)
{
  if (xmlTextWriterStartElement(writer, BAD_CAST "message") < 0)
    {
      fprintf(stderr, "error from open message\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "file", 
				  BAD_CAST btl->file) < 0)
    {
      fprintf(stderr, "error setting file attribute\n");
      return 1;
    }

  char linestring[32];

  if (snprintf(linestring, 32, "%d", btl->line) >= 32)
    {
      fprintf(stderr, "buffer overflow formatting line number\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "line", 
				  BAD_CAST linestring) < 0)
    {
      fprintf(stderr, "error setting file attribute\n");
      return 1;
    }

  if (xmlTextWriterWriteString(writer, BAD_CAST btl->message) < 0)
    {
      fprintf(stderr, "error writing message body\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr, "error from close message\n");
      return 1;
    }

  return 0;
}

static int lines_print_xml(xmlTextWriter* writer, btrace_line_t *btl)
{
  if (btl)
    {
      return 
	lines_print_xml(writer, btl->next) +
	line_print_xml(writer, btl);
    }  

  return 0;
}

static int print_xml_doc(xmlTextWriter* writer, btrace_t *bt)
{
  if (xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0)
    {
      fprintf(stderr, "error from start document\n");
      return 1;
    }

  if (xmlTextWriterStartElement(writer, BAD_CAST "backtrace") < 0)
    {
      fprintf(stderr, "error from open backtrace\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "program", 
				  BAD_CAST bt->program) < 0)
    {
      fprintf(stderr, "error setting program attribute\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "version", 
				  BAD_CAST VERSION) < 0)
    {
      fprintf(stderr, "error setting file attribute\n");
      return 1;
    }

  if (xmlTextWriterWriteAttribute(writer, 
				  BAD_CAST "created", 
				  BAD_CAST date_string()) < 0)
    {
      fprintf(stderr, "error setting created attribute\n");
      return 1;
    }

  if (lines_print_xml(writer, bt->lines) != 0)
    {
      fprintf(stderr, "error writing lines\n");
      return 1;
    }

  if (xmlTextWriterEndElement(writer) < 0)
    {
      fprintf(stderr, "error from close backtrace\n");
      return 1;
    }

  if (xmlTextWriterEndDocument(writer) < 0)
    {
      fprintf(stderr, "error from end document\n");
      return 1;
    }

  return 0;
}

static int print_xml(FILE *stream, btrace_t *bt)
{
  if (is_empty(bt))
    return 0;

  xmlBuffer* buffer;
  int err = 0;
            
  if ((buffer = xmlBufferCreate()) != NULL)
    {
      xmlTextWriter* writer;
      
      if ((writer = xmlNewTextWriterMemory(buffer, 0)) != NULL) 
	{
	  if (print_xml_doc(writer, bt) == 0)
	    {
	      fprintf(stream, "%s", buffer->content);
	    }
	  else
	    {
	      fprintf(stderr, "failed to print XML\n");
	      err++;
	    }
	  xmlFreeTextWriter(writer);
	}
      else
	{
	  fprintf(stderr, "failed to create XML writer\n");
	  err++;
	}
      xmlBufferFree(buffer);
    }
  else
    {
      fprintf(stderr, "failed to create XML buffer\n");
      err++;
    }

  return err;
}

#ifdef HAVE_JANSSON_H

/*
  print JSON format
*/

static int line_print_json(json_t *msgs, btrace_line_t *btl)
{
  json_t *msg = json_object();

  json_object_set_new(msg, "file", json_string(btl->file));
  json_object_set_new(msg, "line", json_integer(btl->line));
  json_object_set_new(msg, "message", json_string(btl->message));

  json_array_append(msgs, msg); 

  return 0;
}

static int lines_print_json(json_t* msgs, btrace_line_t *btl)
{
  if (btl)
    {
      return 
	lines_print_json(msgs, btl->next) +
	line_print_json(msgs, btl);
    }  

  return 0;
}

static int print_json(FILE *stream, btrace_t *bt)
{
  int err = 0;

  if (is_empty(bt))
    return 0;

  json_t *messages;

  if ((messages = json_array()) != NULL)
    {
      if (lines_print_json(messages, bt->lines) == 0)
	{
	  json_t *root;

	  if ((root = json_object()) != NULL)
	    {
	      if (
		  (json_object_set_new(root, "program", json_string(bt->program)) == 0) &&
		  (json_object_set_new(root, "version", json_string(VERSION)) == 0) &&
		  (json_object_set_new(root, "created", json_string(date_string())) == 0) &&
		  (json_object_set(root, "messages", messages) == 0)
		  )
		{
		  if (json_dumpf(root, stream, JSON_INDENT(2)) == 0)
		    {
		      /* success */
		    }
		  else
		    {
		      fprintf(stderr, "failed to dump to stream\n");
		      err++;
		    }
		}
	      else
		{
		  fprintf(stderr, "error creating JSON message object\n");
		  err++;
		}

	      json_decref(root);
	    }
	  else
	    {
	      fprintf(stderr, "failed to create JSON root object\n");
	      err++;
	    }
	}
      else
	{
	  fprintf(stderr, "failed to convert lines to JSON\n");
	  err++;
	}
      json_decref(messages);
    }
  else
    {
      fprintf(stderr, "failed create JSOM message array\n");
      err++;
    }

  return err;
}

#else

static int print_json(FILE *stream, btrace_t *bt)
{
  fprintf(stderr, "compiled without jansson library support\n");
  return 1;
}

#endif

typedef int (*printer_t)(FILE*, btrace_t*);

extern int btrace_print_stream(FILE* stream, int type)
{
  printer_t printer = NULL;

  switch (type)
    {
    case BTRACE_PLAIN: 
      printer = print_plain;
      break;
    case BTRACE_XML:
      printer = print_xml;
      break;
    case BTRACE_JSON:
      printer = print_json;
      break;
    default:
      fprintf(stderr, "no such trace format\n");
      return 1;
    }

  return printer(stream, &btrace_global);
}

extern int btrace_print(const char* path, int type)
{
  if (! btrace_is_empty() )
    {
      FILE* stream;

      if ((stream = fopen(path, "w")) == NULL)
	return 1;
      
      btrace_print_stream(stream, type);

      if (fclose(stream) != 0)
	return 1;
    }

  return 0;
}
