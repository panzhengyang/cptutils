/*
  findgrad.h
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
  $Id: findgrad.h,v 1.1 2002/06/18 22:25:44 jjg Exp jjg $
*/

#ifndef FINDGRAD_H 
#define FINDGRAD_H

extern char* findgrad_explicit(char*);
extern char* findgrad_indir(char*,char*);
extern char* findgrad_implicit(char*);

#endif
