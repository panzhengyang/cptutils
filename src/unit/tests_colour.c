/*
  cunit tests for colour.c
  J.J.Green 2014,
*/

#include <colour.h>
#include "tests_colour.h"
#include "tests_colour_helper.h"

/* byte quantisation error */

#define OCTEPS (1.0 / 256.0)

CU_TestInfo tests_colour[] = 
  {
    {"hsvD to rgbD",     test_colour_hsvD_to_rgbD},
    {"rgbD to hsvD",     test_colour_rgbD_to_hsvD},
    {"rgb to rgbD",      test_colour_rgb_to_rgbD},
    {"rgbD to rgb",      test_colour_rgbD_to_rgb},
    {"hsv to rgbD",      test_colour_hsv_to_rgbD},
    {"rgbD to hsv",      test_colour_rgbD_to_hsv},
    {"grey to rgbD",     test_colour_grey_to_rgbD},
    {"parse RGB string", test_colour_parse_rgb},
    {"rgb interpolate",  test_colour_rgb_interpolate},
    {"hsv interpolate",  test_colour_hsv_interpolate},
    CU_TEST_INFO_NULL,
  };


extern void test_colour_hsvD_to_rgbD(void)
{
  double eps = 1e-10, rgb[3];

  {
    double hsv[3] = {0.0, 0.5, 1.0}, rgb_expected[3] = {1.0, 0.5, 0.5};

    CU_ASSERT(hsvD_to_rgbD(hsv, rgb) == 0);
    CU_ASSERT(triple_equal(rgb, rgb_expected, eps));
  }

  {
    double hsv[3] = {0.0, 0.0, 1.0}, rgb_expected[3] = {1.0, 1.0, 1.0};

    CU_ASSERT(hsvD_to_rgbD(hsv, rgb) == 0);
    CU_ASSERT(triple_equal(rgb, rgb_expected, eps));
  }
}

extern void test_colour_rgbD_to_hsvD(void)
{
  double eps = 1e-10, hsv[3];

  {
    double rgb[3] = {1.0, 0.5, 0.5}, hsv_expected[3] = {0.0, 0.5, 1.0};

    CU_ASSERT(rgbD_to_hsvD(rgb, hsv) == 0);
    CU_ASSERT(triple_equal(hsv, hsv_expected, eps));
  }

  {
    double rgb[3] = {1.0, 1.0, 1.0}, hsv_expected[3] = {0.0, 0.0, 1.0};
      
    CU_ASSERT(rgbD_to_hsvD(rgb, hsv) == 0);
    CU_ASSERT(triple_equal(hsv, hsv_expected, eps));
  }
}

extern void test_colour_rgbD_to_rgb(void)
{
  double rgbD[3] = {0, 0.5, 1};
  rgb_t rgb, rgb_expected = {0, 128, 255};

  CU_ASSERT(rgbD_to_rgb(rgbD, &rgb) == 0);
  CU_ASSERT(rgb_equal(rgb, rgb_expected));
}

extern void test_colour_rgb_to_rgbD(void)
{
  rgb_t rgb = {0, 128, 255};
  double eps = OCTEPS, rgbD[3], 
    rgbD_expected[3] = {0, 0.5, 1};

  CU_ASSERT(rgb_to_rgbD(rgb, rgbD) == 0);
  CU_ASSERT(triple_equal(rgbD, rgbD_expected, eps));
}

extern void test_colour_hsv_to_rgbD(void)
{
  hsv_t hsv = {0.0, 0.5, 1.0};
  double eps = OCTEPS, rgbD[3], 
    rgbD_expected[3] = {1.0, 0.5, 0.5};

  CU_ASSERT(hsv_to_rgbD(hsv, rgbD) == 0);
  CU_ASSERT(triple_equal(rgbD, rgbD_expected, eps));
}

extern void test_colour_rgbD_to_hsv(void)
{
  hsv_t hsv, hsv_expected = {0.0, 0.5, 1.0};
  double eps = OCTEPS, rgbD[3] = {1.0, 0.5, 0.5};

  CU_ASSERT(rgbD_to_hsv(rgbD, &hsv) == 0);
  CU_ASSERT(hsv_equal(hsv, hsv_expected, eps));
}

extern void test_colour_grey_to_rgbD(void)
{
  int grey = 128;
  double eps = OCTEPS, rgbD[3], 
    rgbD_expected[3] = {0.5, 0.5, 0.5};

  CU_ASSERT(grey_to_rgbD(grey, rgbD) == 0);
  CU_ASSERT(triple_equal(rgbD, rgbD_expected, eps));
}

extern void test_colour_parse_rgb(void)
{
  rgb_t rgb, rgb_expected = {10, 50, 200};

  CU_ASSERT(parse_rgb("10/50/200", &rgb) == 0);
  CU_ASSERT(rgb_equal(rgb, rgb_expected))

  CU_ASSERT(parse_rgb("10/50", &rgb) != 0);
}

extern void test_colour_rgb_interpolate(void)
{
  rgb_t rgb,
    rgb1 = build_rgb(0, 1, 2),
    rgb2 = build_rgb(2, 2, 2),
    rgb3 = build_rgb(4, 3, 2);
    
  CU_ASSERT_EQUAL(rgb_interpolate(0.0, rgb1, rgb3, &rgb), 0);
  CU_ASSERT_EQUAL(rgb_equal(rgb, rgb1), 1);
  CU_ASSERT_EQUAL(rgb_interpolate(0.5, rgb1, rgb3, &rgb), 0);
  CU_ASSERT_EQUAL(rgb_equal(rgb, rgb2), 1);
  CU_ASSERT_EQUAL(rgb_interpolate(1.0, rgb1, rgb3, &rgb), 0);
  CU_ASSERT_EQUAL(rgb_equal(rgb, rgb3), 1);
}

extern void test_colour_hsv_interpolate(void)
{
  double eps = OCTEPS;
  hsv_t hsv,
    hsv1 = build_hsv(0.0,     0.5,     1.0),
    hsv2 = build_hsv(0.0, 2.0/3.0, 3.0/4.0),
    hsv3 = build_hsv(0.0,     1.0,     0.5);

  CU_ASSERT(hsv_interpolate(0.0, hsv1, hsv3, &hsv) == 0);
  CU_ASSERT(hsv_equal(hsv, hsv1, eps));
  CU_ASSERT(hsv_interpolate(0.5, hsv1, hsv3, &hsv) == 0);
  CU_ASSERT(hsv_equal(hsv, hsv2, eps));
  CU_ASSERT(hsv_interpolate(1.0, hsv1, hsv3, &hsv) == 0);
  CU_ASSERT(hsv_equal(hsv, hsv3, eps));
}

