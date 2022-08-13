#include "iwstart.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef IW_EXEC
int main(int argc, const char *argv[]) {
  fprintf(stderr, "Hello!!!\n");
  return EXIT_SUCCESS;
}
#endif
