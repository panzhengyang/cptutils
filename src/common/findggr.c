/*
  findgrad.c
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
  $Id: findgrad.c,v 1.1 2002/06/18 22:25:32 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "files.h"
#include "findgrad.h"

#define BUFFSIZE 500

char buffer [BUFFSIZE];
 
extern char* findgrad_explicit(char* name,char* dir)
{
    char* explicit;

    if (!name) return NULL;

    if (dir)
    {
	if (snprintf(buffer,BUFFSIZE,"%s%c%s",dir,DIRSEP,name) == -1)
	    return NULL;

	explicit = buffer;
    }
    else 
	explicit = name;

    return (file_readable(explicit) ? strdup(explicit) : NULL);
}

/*
  the glob expressions here are a bit rough, perhaps it could
  be redone with proper regexps
*/

extern char* findgrad_implicit(char* name)
{
    char  *home,
	  *found=NULL,
	  *dir,
	  *dirs[] = {"/usr/local/share/gimp","/usr/share/gimp",NULL};
    glob_t globdata;
    int    i;

    /* check the home directory */

    if ((home = getenv("HOME")) != NULL)
    {
	snprintf(buffer,BUFFSIZE,"%s/.gimp-*.*/gradients/%s*",home,name);
	glob(buffer,0,NULL,&globdata);

	for (i=0 ; i<globdata.gl_pathc && !found ; i++)
	{
	    char* path;

	    path = globdata.gl_pathv[i];

	    if (file_readable(path)) found = strdup(path);
	}

	globfree(&globdata);
    }

    /*
      now look in the usual places 
    */

    for (i=0 ; (dir = dirs[i]) && !found ; i++)
    {
	int j;

	snprintf(buffer,BUFFSIZE,"%s/*.*/gradients/%s*",dir,name);
	glob(buffer,0,NULL,&globdata);

	for (j=0 ; j<globdata.gl_pathc && !found; j++)
	{
	    char* path;

	    path = globdata.gl_pathv[j];

	    if (file_readable(path)) found = strdup(path);
	}

	globfree(&globdata);
    }

    return found;
}






