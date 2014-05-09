#include <stdio.h>
#include <unistd.h>

#include <css3write.h>
#include "tests_css3write.h"

CU_TestInfo tests_css3write[] = 
  {
    {"simple", test_css3write_simple},
    CU_TEST_INFO_NULL,
  };

/* build a css3 and write it  */

extern void test_css3write_simple(void)
{
  css3_t *css3; 

  CU_ASSERT((css3 = css3_new()) != NULL);
  CU_ASSERT(css3_stops_alloc(css3, 2) == 0);
  CU_ASSERT(css3->n == 2);
  CU_ASSERT(css3->angle == 0);

  css3_stop_t stops[2] =
    {
      {  0, 1, {128,  64,   0}},
      {100, 1, {  0, 128, 255}}
    };

  size_t i;

  for (i=0 ; i<2 ; i++)
    css3->stop[i] = stops[i];

  char *path = tmpnam(NULL);  
  CU_ASSERT(css3_write(path, css3) == 0);
  unlink(path);
  css3_destroy(css3);
}
