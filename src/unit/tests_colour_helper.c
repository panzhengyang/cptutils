#include <stdlib.h>
#include <math.h>

#include "tests_colour_helper.h"

extern bool rgb_equal(rgb_t a, rgb_t b)
{
  return 
    (a.red   == b.red)   && 
    (a.green == b.green) && 
    (a.blue  == b.blue);
}

#define HSVD3(X) {(X).hue, (X).sat, (X).val}

extern bool hsv_equal(hsv_t a, hsv_t b, double eps)
{
  double A[3] = HSVD3(a), B[3] = HSVD3(b);

  return triple_equal(A, B, eps);
}

extern bool triple_equal(double *a, double *b, double eps)
{
  size_t i;

  for (i=0 ; i<3 ; i++)
    {
      if (fabs(a[i] - b[i]) > eps)
	return false;
    }

  return true;
}
