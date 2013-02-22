/*
  gimpcpt.c

  (c) J.J.Green 2001,2004
  $Id: gimplut.c,v 1.3 2012/03/09 22:15:13 jjg Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gimplut.h"
#include "ggr.h"
#include "findggr.h"
#include "files.h"

/*
  #define DEBUG
*/

#define SCALE(x,opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

#define ERR_CMOD   1
#define ERR_NULL   2
#define ERR_INSERT 3

static char* find_infile(char*);
static int gimplut_st(FILE*,gradient_t*,glopt_t);

extern int gimplut(char* infile,char* outfile,glopt_t opt)
{
    gradient_t* gradient;
    int         err;

    /* get the full filename */
    
    if (infile)
      {
	char* found = NULL;
	
	found = find_infile(infile);
	
	if (found)
	  {
	    if (opt.verbose) printf("gradient file %s\n",found);
	  }
	else
	  {
	    fprintf(stderr,"gradient file %s not found\n",infile);
	    return 1;
	  }
	
	infile = found;
      }
    
    /* load the gradient */
    
    gradient = grad_load_gradient(infile);
    
    if (!gradient)
      {
	fprintf(stderr,"failed to load gradient from ");
	if (infile)
	  {
	    fprintf(stderr,"%s\n",infile);
	    free(infile);
	  }
	else
	  fprintf(stderr,"<stdin>\n");
	
	return 1;
      }
    
    if (outfile)
      {
	FILE *lutst = fopen(outfile,"w");

	if (!lutst)
	  {
	    fprintf(stderr,"failed to open %s\n",outfile);
	    return 1;
	  }

	err = gimplut_st(lutst,gradient,opt);

	fclose(lutst);
      }
    else err = gimplut_st(stdout,gradient,opt);

    if ((!err) && opt.verbose) 
      printf("converted to %zu entry LUT\n",opt.numsamp);
    
    grad_free_gradient(gradient);
    
    return err;
}

static char* find_infile(char* infile)
{
  char  *gimp_grads,*found;

  /* try just the name */
  
  found = findggr_explicit(infile);

  if (found) return found;
  else if (absolute_filename(infile)) return NULL;
  
  /* check the GIMP_GRADIENTS directories */
  
  if ((gimp_grads = getenv("GIMP_GRADIENTS")) != NULL)
    {
      char* dir;
      
      if ((gimp_grads = strdup(gimp_grads)) == NULL)
	return NULL;
      
      dir = strtok(gimp_grads,":");
      while (dir && !found)
	{
	  found = findggr_indir(infile,dir);
	  dir = strtok(NULL,":");
	} 
      free(gimp_grads);
      
      if (found) return found;
    }
  
  /* now try the usual places */
  
  return findggr_implicit(infile);
}

static int gimplut_st(FILE* st,gradient_t* g,glopt_t opt)
{
  size_t i, n=opt.numsamp, err = 0;
  double bg[3] = {0.0, 0.0, 0.0}, c[3];
  rgb_t rgb;
  char lut[n*3];

  for (i=0 ; i<n ; i++)
    {
      double pos = ((double)i)/((double)(n-1));

      err += gradient_colour(pos,g,bg,c);
      err += rgbD_to_rgb(c,&rgb);

      lut[i]     = rgb.red;
      lut[n+i]   = rgb.green;
      lut[2*n+i] = rgb.blue;
    }

  if (fwrite(lut,1,3*n,st) != 3*n) err++;

  return err != 0;
}

