/*
  cunit tests for cpt.c
  J.J.Green 2014,
*/

#include <fill.h>
#include "tests_fill_helper.h"
#include "tests_fill.h"

CU_TestInfo tests_fill[] = 
  {
    {"equality", test_fill_eq},
    {"conversion to RGB", test_fill_rgb},
    {"interpolation", test_fill_interp},
    CU_TEST_INFO_NULL,
  };

extern void test_fill_eq(void)
{
  fill_t 
    g1 = build_fill_grey(1),
    g2 = build_fill_grey(2),
    rgb1 = build_fill_rgb(1, 1, 1),
    rgb2 = build_fill_rgb(2, 2, 2);

  CU_ASSERT_EQUAL(fill_eq(g1, g1), 1);
  CU_ASSERT_EQUAL(fill_eq(g1, g2), 0);
  CU_ASSERT_EQUAL(fill_eq(g2, g1), 0);

  CU_ASSERT_EQUAL(fill_eq(g1, rgb1), 0);
  CU_ASSERT_EQUAL(fill_eq(rgb1, g1), 0);

  CU_ASSERT_EQUAL(fill_eq(rgb1, rgb1), 1);
  CU_ASSERT_EQUAL(fill_eq(rgb2, rgb1), 0);
  CU_ASSERT_EQUAL(fill_eq(rgb1, rgb2), 0);
}

static void test_fill_rgb_rgb(void)
{
  rgb_t rgb;
  fill_t fill = build_fill_rgb(1, 2, 3);

  CU_ASSERT_EQUAL(fill_rgb(fill, model_rgb, &rgb), 0);
  CU_ASSERT_EQUAL(rgb.red,   1);
  CU_ASSERT_EQUAL(rgb.green, 2);
  CU_ASSERT_EQUAL(rgb.blue,  3);
}

static void test_fill_rgb_hsv_0_val(void)
{
  rgb_t rgb;
  fill_t fill = build_fill_hsv(150.0, 0.0, 0.0);

  CU_ASSERT_EQUAL(fill_rgb(fill, model_hsv, &rgb), 0);
  CU_ASSERT_EQUAL(rgb.red,   0);
  CU_ASSERT_EQUAL(rgb.green, 0);
  CU_ASSERT_EQUAL(rgb.blue,  0);
}

static void test_fill_rgb_hsv_100_val(void)
{
  rgb_t rgb;
  fill_t fill = build_fill_hsv(0.0, 0.0, 100.0);

  CU_ASSERT_EQUAL(fill_rgb(fill, model_hsv, &rgb), 0);
  CU_ASSERT_EQUAL(rgb.red,   255);
  CU_ASSERT_EQUAL(rgb.green, 255);
  CU_ASSERT_EQUAL(rgb.blue,  255);
}

extern void test_fill_rgb(void)
{
  test_fill_rgb_rgb();
  test_fill_rgb_hsv_0_val();
  test_fill_rgb_hsv_100_val();
}

static void test_fill_interp_grey(void)
{
  fill_t g,
    g1 = build_fill_grey(1),
    g2 = build_fill_grey(2),
    g3 = build_fill_grey(3);
    
  CU_ASSERT_EQUAL(fill_interpolate(0.0, g1, g3, model_rgb, &g), 0);
  CU_ASSERT_EQUAL(fill_eq(g, g1), 1);
  CU_ASSERT_EQUAL(fill_interpolate(0.5, g1, g3, model_rgb, &g), 0);
  CU_ASSERT_EQUAL(fill_eq(g, g2), 1);
  CU_ASSERT_EQUAL(fill_interpolate(1.0, g1, g3, model_rgb, &g), 0);
  CU_ASSERT_EQUAL(fill_eq(g, g3), 1);
}

static void test_fill_interp_rgb(void)
{
  fill_t rgb,
    rgb1 = build_fill_rgb(1, 1, 1),
    rgb2 = build_fill_rgb(2, 2, 2),
    rgb3 = build_fill_rgb(3, 3, 3);
    
  CU_ASSERT_EQUAL(fill_interpolate(0.0, rgb1, rgb3, model_rgb, &rgb), 0);
  CU_ASSERT_EQUAL(fill_eq(rgb, rgb1), 1);
  CU_ASSERT_EQUAL(fill_interpolate(0.5, rgb1, rgb3, model_rgb, &rgb), 0);
  CU_ASSERT_EQUAL(fill_eq(rgb, rgb2), 1);
  CU_ASSERT_EQUAL(fill_interpolate(1.0, rgb1, rgb3, model_rgb, &rgb), 0);
  CU_ASSERT_EQUAL(fill_eq(rgb, rgb3), 1);
}

extern void test_fill_interp(void)
{
  test_fill_interp_grey();
  test_fill_interp_rgb();
}
