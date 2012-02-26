/*
  cptinfo.h - summary information on a cpt file
*/

#ifndef CPTINFO_H
#define CPTINFO_H

typedef enum { false=0, true } bool_t;
typedef enum { plain, csv } format_t;

typedef struct
{
  struct
  {
    char *input,*output;
  } file;
  bool_t verbose;
  format_t format;
} cptinfo_opt_t;

extern int cptinfo(cptinfo_opt_t); 

#endif
