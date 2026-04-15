
#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(const int argc, char *const *const argv) {

  if (argc < 0) {
    // why does the LibC ABI use int and not unsigned int for this xD
    fprintf(stderr, "What the fuck just happened?\n");
    return 1;
  }

  char **const array = malloc(sizeof(char *) * (argc + 3));

  if (array == NULL) {
    fprintf(stderr, "OOM\n");
    return 1;
  }

  array[0] = NODE_PATH;
  array[1] = EXEC_DIR;
  for (size_t i = 0; i < (size_t)argc; ++i) {
    array[2 + i] = argv[i];
  }

  array[argc + 2] = NULL;

  execv(NODE_PATH, array);

  fprintf(stderr, "rapper script failed with: %s\n", strerror(errno));
  return 1;
}
