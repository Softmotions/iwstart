#include "iwstart.h"

#include "cmake_iowow_add.inc"
#include "cmake_iwnet_add.inc"
#include "cmake_ejdb2_add.inc"

#include "cmake_lists.inc"
#include "src_cmake_lists.inc"
#include "tests_cmake_lists.inc"

#include "app_h.inc"
#include "app_c.inc"
#include "app_run.inc"
#include "test1.inc"

#include "app_json.inc"
#include "readme_md.inc"

#include <iowow/iwp.h>

#include <stdio.h>

//const char* _


iwrc _install(const unsigned char *data, const unsigned int len, const char *dir) {
  iwrc rc = 0;

  return rc;
}


iwrc iws_run(void) {
  iwrc rc = 0;
  fprintf(stderr, "Hello\n");
  return 0;
}
