/*
  gpt.h
  structures for gnuplot colour maps

  J.J.Green 2010
  $Id: pov.h,v 1.7 2006/08/27 23:34:24 jjg Exp $
*/

#ifndef GPT_H
#define GPT_H

typedef struct 
{
  double z;
  unsigned char rgb[3];   
} gpt_stop_t;

typedef struct 
{
  int n;
  gpt_stop_t *stop;
} gpt_t;

extern gpt_t* gpt_new(void);
extern void gpt_destroy(gpt_t*);

extern int gpt_stops_alloc(gpt_t*,int);

#endif
