#include "@project_artifact@.h"

#include <stdio.h>

iwrc run(void) {
  iwrc rc = 0;
  fprintf(stderr, "Hello!\n");
  return rc;
}
