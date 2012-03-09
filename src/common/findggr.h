/*
  findggr.h
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
  $Id: findggr.h,v 1.3 2011/11/11 15:40:36 jjg Exp jjg $
*/

#ifndef FINDGGR_H 
#define FINDGGR_H

extern char* findggr_explicit(const char*);
extern char* findggr_indir(const char*, const char*);
extern char* findggr_implicit(const char*);

#endif
