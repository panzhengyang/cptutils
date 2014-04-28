#include "tests_cpt_helper.h"

extern cpt_seg_t* build_segment(fill_t f0, fill_t f1, double z0, double z1)
{
  cpt_seg_t *cpt_seg = cpt_seg_new();

  cpt_seg->lsmp.val = z0;
  cpt_seg->rsmp.val = z1;

  cpt_seg->lsmp.fill = f0;
  cpt_seg->rsmp.fill = f1;

  return cpt_seg;
}
