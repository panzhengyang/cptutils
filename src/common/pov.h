/*
  pov.h
  structures for povray colour maps

  J.J.Green 2005
  $Id: pov.h,v 1.3 2005/09/20 23:00:58 jjg Exp jjg $
*/

#ifndef POV_H
#define POV_H

/* piss poor limitatation! */

#define POV_NAME_LEN 256

typedef struct pov_stop_t
{
  double z;
  double rgbt[4];   
} pov_stop_t;

typedef struct pov_t
{
  char name[POV_NAME_LEN];
  int n;
  pov_stop_t *stop;
} pov_t;

extern pov_t* pov_new(void);
extern void pov_destroy(pov_t*);

extern int pov_stops_alloc(pov_t*,int);
extern int pov_set_name(pov_t*,const char*);

#endif
