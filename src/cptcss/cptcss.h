/*
  cptcss.h - summary cssrmation on a cpt file
  $Id$
*/

#ifndef CPTCSS_H
#define CPTCSS_H

typedef enum { false=0, true } bool_t;

typedef struct
{
  bool_t      verbose;
  const char *cpt;
  const char *format;
} cptcss_opt_t;

extern int cptcss(cptcss_opt_t); 

#endif
