#include "tests_fill_helper.h"

extern fill_t build_fill_grey(int g)
{
  fill_t fill = {
    .type = fill_grey,
    .u = {
      .grey = g
    }
  };
  return fill;
}

extern fill_t build_fill_rgb(int r, int g, int b)
{
  fill_t fill = {
    .type = fill_colour,
    .u = {
      .colour = {
	.rgb = {
	  .red   = r,
	  .green = g,
	  .blue  = b
	}
      }
    }
  };
  return fill;
}

extern fill_t build_fill_hsv(double h, double s, double v)
{
  fill_t fill = {
    .type = fill_colour,
    .u = {
      .colour = {
	.hsv = {
	  .hue = h,
	  .sat = s,
	  .val = v
	}
      }
    }
  };
  return fill;
}
