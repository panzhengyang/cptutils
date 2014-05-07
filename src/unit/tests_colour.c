/*
  cunit tests for colour.c
  J.J.Green 2014,
*/

#include <colour.h>
#include "tests_colour.h"
#include "tests_colour_helper.h"

CU_TestInfo tests_colour[] = 
  {
    {"hsvD_to_rgbD", test_colour_hsvD_to_rgbD},
    {"rgbD_to_hsvD", test_colour_rgbD_to_hsvD},
    {"parse RGB string", test_colour_parse_rgb},
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

extern void test_colour_parse_rgb(void)
{
  rgb_t rgb, rgb_expected = {10, 50, 200};

  CU_ASSERT(parse_rgb("10/50/200", &rgb) == 0);
  CU_ASSERT(rgb_equal(rgb, rgb_expected))

  CU_ASSERT(parse_rgb("10/50", &rgb) != 0);
}


