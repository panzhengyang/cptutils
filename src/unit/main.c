#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <CUnit/Basic.h>

#include "options.h"
#include "tests.h"

#define NO_STDERR

int main(int argc, char** argv)
{
  struct gengetopt_args_info info;

  if (options(argc, argv, &info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }
  bool verbose = info.verbose_given;

  CU_BasicRunMode mode = (verbose ? CU_BRM_VERBOSE : CU_BRM_SILENT);
  CU_ErrorAction error_action = CUEA_IGNORE;
  setvbuf(stdout, NULL, _IONBF, 0);

  if (CU_initialize_registry()) 
    {
      fprintf(stderr,"failed to initialise registry\n");
      return EXIT_FAILURE;
    }

  tests_load();
  CU_basic_set_mode(mode);
  CU_set_error_action(error_action);

  int status;

  /*
    suppress stderr during the tests, this is not 
    that portable and we might need some autoconf 
  */

#ifdef NO_STDERR

  if (freopen("/dev/null", "w", stderr) == NULL)
    {
      printf("failed to redirect stderr\n");
      return EXIT_FAILURE;
    }

#endif

  status = CU_basic_run_tests();

#ifdef NO_STDERR

  if (freopen("/dev/stderr", "w", stderr) == NULL)
    {
      printf("failed to redirect stderr back\n");
      return EXIT_FAILURE;
    }

#endif

  int nfail = CU_get_number_of_failures();

  if (verbose)
    printf("\nSuite %s: %d failed\n", 
	   (status == 0 ? "OK" : "errored"),
	   nfail);

  CU_cleanup_registry();
  options_free(&info);

  return (nfail > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
