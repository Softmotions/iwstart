#include "iwstart.h"

#include <iowow/iwlog.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef IW_EXEC

int main(int argc, const char *argv[]) {
  iwrc rc = 0;
  RCC(rc, finish, iw_init());

  fprintf(stderr, "Hello!!!\n");

finish:
  if (rc) {
    iwlog_ecode_error3(rc);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#endif
