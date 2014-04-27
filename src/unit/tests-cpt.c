/*
  cunit tests for cpt.c
  J.J.Green 2014
*/

#include <cpt.h>
#include "tests-cpt.h"

CU_TestInfo tests_cpt[] = 
  {
    {"cpt constructor", test_cpt_new},
    {"cpt segment constructor", test_cpt_seg_new},
    {"cpt segment append", test_cpt_append},
    {"cpt segment pop", test_cpt_pop},
    {"cpt segment shift", test_cpt_shift},
    {"cpt zrange", test_cpt_zrange},
    {"cpt zfill (grey)", test_cpt_zfill_grey},
    CU_TEST_INFO_NULL,
  };

extern void test_cpt_new(void)
{
  cpt_t *cpt = cpt_new();

  CU_ASSERT_NOT_EQUAL(cpt, NULL);
  CU_ASSERT_EQUAL(cpt->name, NULL);
  CU_ASSERT_EQUAL(cpt->model, model_rgb);
  CU_ASSERT_EQUAL(cpt->segment, NULL);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 0);

  cpt_destroy(cpt);
}

extern void test_cpt_seg_new(void)
{
  cpt_seg_t *cpt_seg = cpt_seg_new();

  CU_ASSERT_NOT_EQUAL(cpt_seg, NULL);

  cpt_seg_destroy(cpt_seg);
}

extern void test_cpt_append(void)
{
  cpt_t *cpt = cpt_new();
  cpt_seg_t *cpt_seg = cpt_seg_new();

  CU_ASSERT_EQUAL(cpt_append(cpt_seg, cpt), 0);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 1);

  cpt_destroy(cpt);
}

static cpt_seg_t* grey_segment(int g, double z0, double z1)
{
  cpt_seg_t *cpt_seg = cpt_seg_new();

  cpt_seg->lsmp.val = z0;
  cpt_seg->rsmp.val = z1;

  fill_t fill = {
    .type = fill_grey,
    .u = {
      .grey = g
    }
  };

  cpt_seg->lsmp.fill = fill;
  cpt_seg->rsmp.fill = fill;

  return cpt_seg;
}

extern void test_cpt_pop(void)
{
  cpt_seg_t 
    *seg1 = grey_segment(200, 0.0, 1.0),
    *seg2 = grey_segment(100, 1.0, 2.0);

  cpt_t *cpt = cpt_new();

  CU_ASSERT_EQUAL(cpt_append(seg1, cpt), 0);
  CU_ASSERT_EQUAL(cpt_append(seg2, cpt), 0);

  CU_ASSERT_EQUAL(cpt_nseg(cpt), 2);

  cpt_seg_t *seg;

  CU_ASSERT_NOT_EQUAL(seg = cpt_pop(cpt), NULL);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 1);
  CU_ASSERT_EQUAL(seg, seg1);

  CU_ASSERT_NOT_EQUAL(seg = cpt_pop(cpt), NULL);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 0);
  CU_ASSERT_EQUAL(seg, seg2);

  cpt_seg_destroy(seg1);
  cpt_seg_destroy(seg2);

  cpt_destroy(cpt);
}

extern void test_cpt_shift(void)
{
  cpt_seg_t 
    *seg1 = grey_segment(200, 0.0, 1.0),
    *seg2 = grey_segment(100, 1.0, 2.0);

  cpt_t *cpt = cpt_new();

  CU_ASSERT_EQUAL(cpt_append(seg1, cpt), 0);
  CU_ASSERT_EQUAL(cpt_append(seg2, cpt), 0);

  CU_ASSERT_EQUAL(cpt_nseg(cpt), 2);

  cpt_seg_t *seg;

  CU_ASSERT_NOT_EQUAL(seg = cpt_shift(cpt), NULL);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 1);
  CU_ASSERT_EQUAL(seg, seg2);

  CU_ASSERT_NOT_EQUAL(seg = cpt_shift(cpt), NULL);
  CU_ASSERT_EQUAL(cpt_nseg(cpt), 0);
  CU_ASSERT_EQUAL(seg, seg1);

  cpt_seg_destroy(seg1);
  cpt_seg_destroy(seg2);

  cpt_destroy(cpt);
}

extern void test_cpt_zrange(void)
{
  cpt_seg_t 
    *seg1 = grey_segment(200, 0.0, 1.0),
    *seg2 = grey_segment(100, 1.0, 2.0);

  cpt_t *cpt = cpt_new();

  CU_ASSERT_EQUAL(cpt_append(seg1, cpt), 0);
  CU_ASSERT_EQUAL(cpt_append(seg2, cpt), 0);

  double z[2];

  CU_ASSERT_EQUAL(cpt_zrange(cpt, z), 0);

  CU_ASSERT_EQUAL(z[0], 0.0);
  CU_ASSERT_EQUAL(z[1], 2.0);

  cpt_destroy(cpt);
}

extern void test_cpt_zfill_grey(void)
{
  cpt_seg_t *seg = grey_segment(200, 0.0, 1.0);
  cpt_t *cpt = cpt_new();
  CU_ASSERT_EQUAL(cpt_append(seg, cpt), 0);

  fill_t fill;
  CU_ASSERT_EQUAL(cpt_zfill(cpt, 0.5, &fill), 0);
  CU_ASSERT_EQUAL(fill.type, fill_grey);
  CU_ASSERT_EQUAL(fill.u.grey, 200);
}

