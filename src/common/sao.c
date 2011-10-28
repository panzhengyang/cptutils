/*
  sao.c

  J.J. Green 2011
  $Id: sao.c,v 1.1 2011/10/27 22:31:26 jjg Exp jjg $
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sao.h"

typedef struct sao_stop_t sao_stop_t;

struct sao_stop_t
{
  double x,v;
  sao_stop_t *next;
};

struct sao_t
{
  sao_stop_t *red, *green, *blue;
};

static sao_stop_t* sao_stop_new(double x, double v)
{
  sao_stop_t *stop;

  if ( (stop = malloc(sizeof(sao_stop_t))) == NULL)
    return NULL;

  stop->x = x, stop->v = v;
  stop->next = NULL;

  return stop;
}

/* not used at present

static int sao_channel_prepend(sao_stop_t** base, double x, double v)
{
  sao_stop_t *stop = sao_stop_new(x,v);
 
  if (! stop)
    return 1;

  stop->next = *base;
  *base = stop;

  return 0;
}

*/

static int sao_channel_append(sao_stop_t** base, double x, double v)
{
  sao_stop_t *s, *stop = sao_stop_new(x,v);
  
  if (! stop)
    return 1;

  if (*base)
    {
      for (s=*base ; s->next ; s=s->next);
      s->next = stop;
    }
  else
    {
      *base = stop;
    }

  return 0;
}

extern int sao_red_push(sao_t *sao, double x, double v)
{
  return sao_channel_append( &(sao->red), x, v);
}

extern int sao_green_push(sao_t *sao, double x, double v)
{
  return sao_channel_append( &(sao->green), x, v);
}

extern int sao_blue_push(sao_t *sao, double x, double v)
{
  return sao_channel_append( &(sao->blue), x, v);
}

extern sao_t* sao_new(void)
{
  sao_t *sao;

  if ((sao = malloc(sizeof(sao_t))) == NULL)
    return NULL;

  sao->red = sao->green = sao->blue = NULL;

  return sao;
}

static void sao_stop_destroy(sao_stop_t* s)
{
  if (s)
    {
      sao_stop_destroy(s->next);
      free(s);
    }
}

extern void sao_destroy(sao_t* sao)
{
  sao_stop_destroy( sao->red );
  sao_stop_destroy( sao->green );
  sao_stop_destroy( sao->blue );
 
  free(sao);
}

static int sao_eachstop(sao_stop_t* stop, 
			int (*f)(double,double,void*), 
			void *opt)
{
  return (stop ? 
	  f(stop->x, stop->v, opt) + sao_eachstop(stop->next,f,opt) :
	  0);
}

extern int sao_eachred(sao_t* sao, stop_fn_t *f, void *opt)
{
  return sao_eachstop( sao->red, f, opt);
}

extern int sao_eachgreen(sao_t* sao, stop_fn_t *f, void *opt)
{
  return sao_eachstop( sao->green, f, opt);
}

extern int sao_eachblue(sao_t* sao, stop_fn_t *f, void *opt)
{
  return sao_eachstop( sao->blue, f, opt);
}

