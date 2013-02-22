/*
  findggr.h
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
  $Id: findggr.h,v 1.4 2012/03/09 22:11:32 jjg Exp $
*/

#ifndef FINDGGR_H 
#define FINDGGR_H

extern char* findggr_explicit(const char*);
extern char* findggr_indir(const char*, const char*);
extern char* findggr_implicit(const char*);

#endif
