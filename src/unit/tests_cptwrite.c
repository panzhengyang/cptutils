/*
  cunit tests for cptwrite.c
  J.J.Green 2014,
*/

#include <stdio.h>
#include <unistd.h>

#include <cptwrite.h>
#include <cptread.h>
#include "fixture.h"
#include "tests_cptwrite.h"

CU_TestInfo tests_cptwrite[] = 
  {
    {"fixtures", test_cptwrite_fixtures},
    CU_TEST_INFO_NULL,
  };

/* write a selection of cpt files */

extern void test_cptwrite_fixtures(void)
{
  size_t n = 1024;
  char buf[n];
  const char* files[] = {
    "blue.cpt",
    "Exxon88.cpt",
    "GMT_gebco.cpt",
    "GMT_haxby.cpt",
    "Onion_Rings.cpt",
    "pakistan.cpt",
    "RdBu_10.cpt",
    "subtle.cpt",
    "test.cpt",
    "tpsfhm.cpt"
  };
  size_t i, nfile = sizeof(files)/sizeof(char*);

  for (i=0 ; i<nfile ; i++)
    {
      cpt_t* cpt;

      if ((cpt = cpt_new()) != NULL)
	{ 
	  if (fixture(buf, n, "cpt", files[i]) < n)
	    {
	      if (cpt_read(buf, cpt) == 0)
		{
		  char *path = tmpnam(NULL);

		  CU_ASSERT(cpt_write(path, cpt) == 0);
		  unlink(path);
		}
	    }
	  cpt_destroy(cpt);
	}
    }
}
