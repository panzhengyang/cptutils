/*
  cptinfo.h - summary information on a cpt file
*/

#ifndef CPTINFO_H
#define CPTINFO_H

typedef enum { false=0, true } bool_t;
typedef enum { html } format_t;

typedef struct
{
  char    *cpt;
  bool_t   verbose;
  format_t format;
  char    *text;
} cpttext_opt_t;

extern int cpttext(cpttext_opt_t); 

#endif
