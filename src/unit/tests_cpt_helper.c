#include "tests_cpt_helper.h"

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

extern cpt_seg_t* build_segment(fill_t f0, fill_t f1, double z0, double z1)
{
  cpt_seg_t *cpt_seg = cpt_seg_new();

  cpt_seg->lsmp.val = z0;
  cpt_seg->rsmp.val = z1;

  cpt_seg->lsmp.fill = f0;
  cpt_seg->rsmp.fill = f1;

  return cpt_seg;
}
