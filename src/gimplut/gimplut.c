/*
  gimpcpt.c
  
  (c) J.J.Green 2001, 2004, 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ggr.h>
#include <btrace.h>

#include "gimplut.h"

#define SCALE(x, opt) ((opt.min) + ((opt.max) - (opt.min))*(x))

#define ERR_CMOD   1
#define ERR_NULL   2
#define ERR_INSERT 3

static int gimplut_st(FILE*, gradient_t*, glopt_t);

extern int gimplut(char* infile, char* outfile, glopt_t opt)
{
  gradient_t* gradient;
  int         err;
  
  /* load the gradient */
  
  gradient = grad_load_gradient(infile);
  
  if (!gradient)
    {
      btrace("failed to load gradient from %s", (infile ? infile : "<stdin>"));
      return 1;
    }
  
  if (outfile)
    {
      FILE *lutst = fopen(outfile, "w");
      
      if (!lutst)
	{
	  btrace("failed to open %s", outfile);
	  return 1;
	}
      
      err = gimplut_st(lutst, gradient, opt);
      
      fclose(lutst);
    }
  else err = gimplut_st(stdout, gradient, opt);
  
  if ((!err) && opt.verbose) 
    printf("converted to %zu entry LUT\n", opt.numsamp);
  
  grad_free_gradient(gradient);
  
  return err;
}


static int gimplut_st(FILE* st, gradient_t* g, glopt_t opt)
{
  size_t i, n = opt.numsamp;
  double bg[3] = {0.0, 0.0, 0.0}, c[3];
  rgb_t rgb;
  char lut[n*3];

  for (i=0 ; i<n ; i++)
    {
      double pos = ((double)i)/((double)(n-1));

      if (gradient_colour(pos, g, bg, c) != 0)
	{
	  btrace("could not get colour at z = %f", pos);
	  return 1;
	}

      if (rgbD_to_rgb(c, &rgb) != 0)
	{
	  btrace("failed convert to rgb for rgb %f/%f/%f",
		 c[0], c[1], c[2]);
	  return 1;
	}

      lut[i]     = rgb.red;
      lut[n+i]   = rgb.green;
      lut[2*n+i] = rgb.blue;
    }

  return fwrite(lut, 1, 3*n, st) != 3*n;
}

