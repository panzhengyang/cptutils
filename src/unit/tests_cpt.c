/*
  cunit tests for cpt.c
  J.J.Green 2014,
*/

#include <cpt.h>
#include "tests_fill_helper.h"
#include "tests_cpt_helper.h"
#include "tests_cpt.h"

CU_TestInfo tests_cpt[] = 
  {
    {"constructor", test_cpt_new},
    {"segment constructor", test_cpt_seg_new},
    {"segment append", test_cpt_append},
    {"segment pop", test_cpt_pop},
    {"segment shift", test_cpt_shift},
    {"zrange", test_cpt_zrange},
    {"zfill (grey)", test_cpt_zfill_grey},
    {"zfill (rgb)", test_cpt_zfill_rgb},
    CU_TEST_INFO_NULL,
  };

extern void test_cpt_new(void)
{
  cpt_t *cpt = cpt_new();

  CU_ASSERT_NOT_EQUAL(cpt, NULL);
  CU_ASSERT_EQUAL(cpt->name, NULL);
  printf("[%i]\n",cpt->model);
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

extern void test_cpt_pop(void)
{
  fill_t 
    grey100 = build_fill_grey(100),
    grey200 = build_fill_grey(200);
  cpt_seg_t 
    *seg1 = build_segment(grey100, grey200, 0.0, 1.0),
    *seg2 = build_segment(grey100, grey200, 1.0, 2.0);

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
  fill_t 
    grey100 = build_fill_grey(100),
    grey200 = build_fill_grey(200);
  cpt_seg_t 
    *seg1 = build_segment(grey100, grey200, 0.0, 1.0),
    *seg2 = build_segment(grey100, grey200, 1.0, 2.0);

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
  fill_t 
    grey100 = build_fill_grey(100),
    grey200 = build_fill_grey(200);
  cpt_seg_t 
    *seg1 = build_segment(grey100, grey200, 0.0, 1.0),
    *seg2 = build_segment(grey100, grey200, 1.0, 2.0);

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
  fill_t 
    grey100 = build_fill_grey(100),
    grey200 = build_fill_grey(200);
  cpt_seg_t *seg = build_segment(grey100, grey200, 0.0, 1.0);

  cpt_t *cpt = cpt_new();
  CU_ASSERT_EQUAL(cpt_append(seg, cpt), 0);

  fill_t fill;
  CU_ASSERT_EQUAL(cpt_zfill(cpt, 0.5, &fill), 0);
  CU_ASSERT_EQUAL(fill.type, fill_grey);
  CU_ASSERT_EQUAL(fill.u.grey, 150);
}

extern void test_cpt_zfill_rgb(void)
{
  fill_t 
    grey100 = build_fill_rgb(100, 100, 100),
    grey200 = build_fill_rgb(200, 200, 200);
  cpt_seg_t *seg = build_segment(grey100, grey200, 0.0, 1.0);

  cpt_t *cpt = cpt_new();
  CU_ASSERT_EQUAL(cpt_append(seg, cpt), 0);

  fill_t fill;
  CU_ASSERT_EQUAL(cpt_zfill(cpt, 0.5, &fill), 0);
  CU_ASSERT_EQUAL(fill.type, fill_colour);
  CU_ASSERT_EQUAL(fill.u.colour.rgb.red, 150);
}

