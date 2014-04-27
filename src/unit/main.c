#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <CUnit/Basic.h>

#include "options.h"
#include "tests.h"

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

  int status = CU_basic_run_tests();
  int nfail = CU_get_number_of_failures();

  if (verbose)
    printf("\nSuite %s: %d failed\n", 
	   (status == 0 ? "OK" : "errored"),
	   nfail);

  CU_cleanup_registry();
  options_free(&info);

  return (nfail > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
