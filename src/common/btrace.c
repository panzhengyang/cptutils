/*
  btrace.c

  Copyright (c) J.J. Green 2014
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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
  bool enabled;
  btrace_line_t *lines;
} btrace_t;

static btrace_t btrace_global = 
  {
    .enabled = false,
    .lines   = NULL
  };

extern void btrace_enable(void)
{
  btrace_global.enabled = true; 
}

extern void btrace_disable(void)
{
  btrace_global.enabled = false; 
}

extern bool btrace_is_enabled(void)
{
  return btrace_global.enabled; 
}

static btrace_line_t* btrace_line_new(const char* file, int line, char *message)
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

static void btrace_register(const char* file, int line, char *message)
{
  btrace_line_t *btl;

  if ((btl = btrace_line_new(file, line, message)) == NULL)
    return;

  btl->next = btrace_global.lines;
  btrace_global.lines = btl;
}

static void btrace_line_free(btrace_line_t *btl)
{
  free(btl->message);
  free(btl);
}

static void btrace_lines_free(btrace_line_t *btl)
{
  if (btl)
    {
      btrace_lines_free(btl->next);
      btrace_line_free(btl);
    }
}

extern void btrace_reset(void)
{
  btrace_lines_free(btrace_global.lines);
  btrace_global.lines = NULL;
}

static size_t btrace_count_lines(btrace_line_t *btl)
{
  return btl == NULL ? 0 : btrace_count_lines(btl->next) + 1;
}

extern size_t btrace_count(void)
{
  return btrace_count_lines(btrace_global.lines);
}

extern void btrace_add_real(const char* file, int line, const char* format, ...)
{
  char buffer[512];
  va_list args;
  
  va_start(args, format);
  vsnprintf(buffer, BUFSZ, format, args);
  va_end(args);

  btrace_register(file, line, buffer);
}

/* print plain format */

static void line_print_plain(FILE *stream, btrace_line_t *btl)
{
  fprintf(stream, "%s (%s %i)\n", btl->message, btl->file, btl->line);
}

static void lines_print_plain(FILE *stream, btrace_line_t *btl)
{
  if (btl)
    {
      lines_print_plain(stream, btl->next);
      line_print_plain(stream, btl);
    }
}

static void print_plain(FILE *stream, btrace_t *bt)
{
  lines_print_plain(stream, bt->lines);
}

extern void btrace_print_plain(FILE *stream)
{
  print_plain(stream, &(btrace_global));
}

/* print XML format */

static void line_print_xml(FILE *stream, btrace_line_t *btl)
{
  fprintf(stream, "<message file='%s' line='%i'>%s</message>\n", 
	  btl->file, btl->line, btl->message);
}

static void lines_print_xml(FILE *stream, btrace_line_t *btl)
{
  if (btl)
    {
      lines_print_xml(stream, btl->next);
      line_print_xml(stream, btl);
    }
}

static void print_xml(FILE *stream, btrace_t *bt)
{
  if (bt->lines)
    {
      fprintf(stream, "<backtrace>\n");
      lines_print_xml(stream, bt->lines);
      fprintf(stream, "</backtrace>\n");
    }
}

extern void btrace_print_xml(FILE *stream)
{
  print_xml(stream, &btrace_global);
}
