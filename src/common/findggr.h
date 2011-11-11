/*
  findgrad.h
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
  $Id: findgrad.h,v 1.2 2004/01/30 00:08:26 jjg Exp jjg $
*/

#ifndef FINDGRAD_H 
#define FINDGRAD_H

extern char* findgrad_explicit(const char*);
extern char* findgrad_indir(const char*, const char*);
extern char* findgrad_implicit(const char*);

#endif
