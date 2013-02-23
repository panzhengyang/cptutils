/*
  findggr.c
  
  Look in the usual places to find gimp gradient 
  files.

  (c) J.J.Geen 2001
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glob.h>

#include "files.h"
#include "findggr.h"

#define BUFFSIZE 512

char buffer[BUFFSIZE];

/*
  find gradient specified explicitly - dont try .ggr
*/

extern char* findggr_explicit(const char* name)
{
  if (!name) return NULL;
     
  return (file_readable(name) ? strdup(name) : NULL);
}

/*
  find gradient in specified directory, with .ggr test
*/

extern char* findggr_indir(const char* name, const char* dir)
{
  if ((!name) || (!dir)) return NULL;
  
  if (snprintf(buffer,BUFFSIZE,"%s/%s.ggr",dir,name) == -1)
    return NULL;
  
  if (file_readable(buffer)) return strdup(buffer);
    
  if (snprintf(buffer,BUFFSIZE,"%s/%s",dir,name) == -1)
    return NULL;
    
  if (file_readable(buffer)) return strdup(buffer);
  
  return NULL;
}

/*
  the glob expressions here are a bit inelegant, perhaps this 
  could all be redone with regexps
*/

extern char* findggr_implicit(const char* name)
{
    char  *home,
	  *found=NULL,
	  *dir,
	  *dirs[] = {"/usr/local/share/gimp","/usr/share/gimp",NULL};
    glob_t globdata;
    int    i;

    /* check the home gimp directories */

    if ((home = getenv("HOME")) != NULL)
    {
	snprintf(buffer,BUFFSIZE,"%s/.gimp-*.*/gradients/%s",home,name);
	glob(buffer,0,NULL,&globdata);
	snprintf(buffer,BUFFSIZE,"%s/.gimp-*.*/gradients/%s.ggr",home,name);
	glob(buffer,GLOB_APPEND,NULL,&globdata);

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

	snprintf(buffer,BUFFSIZE,"%s/*.*/gradients/%s",dir,name);
	glob(buffer,0,NULL,&globdata);
	snprintf(buffer,BUFFSIZE,"%s/*.*/gradients/%s.ggr",dir,name);
	glob(buffer,GLOB_APPEND,NULL,&globdata);

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






