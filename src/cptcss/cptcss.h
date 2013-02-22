/*
  cptcss.h - summary cssrmation on a cpt file
  $Id: cptcss.h,v 1.1 2005/07/19 22:20:16 jjg Exp $
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
