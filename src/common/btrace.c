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

/* enable/disable */

static void enable(btrace_t *bt)
{
  bt->enabled = true;
}

extern void btrace_enable(void)
{
  enable(&btrace_global); 
}

static void disable(btrace_t *bt)
{
  bt->enabled = false;
}

extern void btrace_disable(void)
{
  disable(&btrace_global);
}

static bool is_enabled(btrace_t *bt)
{
  return bt->enabled;
}

extern bool btrace_is_enabled(void)
{
  return is_enabled(&btrace_global); 
}

/* testing nonempty */

static bool nonempty(btrace_t* bt)
{
  return bt->lines != NULL;
}

extern bool btrace_nonempty(void)
{
  return nonempty(&btrace_global);
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

extern void btrace_add_real(const char* file, int line, const char* format, ...)
{
  char buffer[512];
  va_list args;
  
  va_start(args, format);
  vsnprintf(buffer, BUFSZ, format, args);
  va_end(args);

  append(&btrace_global, file, line, buffer);
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

extern int btrace_print(const char* path, int type)
{
  if (btrace_nonempty())
    {
      void (*f)(FILE *);

      switch (type)
	{
	case BTRACE_PLAIN: 
	  f = btrace_print_plain;
	  break;
	case BTRACE_XML:
	  f = btrace_print_xml;
	  break;
	default:
	  return 1;
	}

      FILE* stream;

      if ((stream = fopen(path, "w")) == NULL)
	return 1;

      f(stream);

      if (fclose(stream) != 0)
	return 1;
    }

  return 0;
}
