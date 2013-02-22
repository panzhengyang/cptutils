/*
  gpt.h
  structures for gnuplot colour maps

  J.J.Green 2010
  $Id: gpt.h,v 1.2 2010/11/01 19:21:28 jjg Exp $
*/

#ifndef GPT_H
#define GPT_H

typedef struct 
{
  double z;
  double rgb[3];   
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
