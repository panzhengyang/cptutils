/*
  pov.h
  structures for povray colour maps

  J.J.Green 2005
  $Id: pov.h,v 1.2 2005/09/20 22:16:01 jjg Exp jjg $
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

extern pov_t* pov_new(int,const char*);
extern void pov_destroy(pov_t*);

#endif
