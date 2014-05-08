#include "tests_colour_helper.h"
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
	.rgb = build_rgb(r, g, b)
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
	.hsv = build_hsv(h, s, v)
      }
    }
  };
  return fill;
}
