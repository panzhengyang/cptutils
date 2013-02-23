/*
  ident.h

  identifiers

  (c) J. J. Green 2002
*/

#ifndef IDENT_H
#define IDENT_H

/* this is an open structure */

typedef struct ident_t
{
  char* name;
  int   id;
} ident_t;

extern ident_t* ident_new(char*,int);
extern void ident_destroy(ident_t*);

#endif
