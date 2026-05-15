
#include "config.h"
#include "git_version.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(const int argc, char* const* const argv) {

	if(argc < 0) {
		// why does the LibC ABI use int and not unsigned int for this xD
		fprintf(stderr, "What the fuck just happened?\n");
		return 1;
	}

	if(argc < 1) {
		fprintf(stderr, "argv[0] is not set?!?!\n");
		return 1;
	}

#define ADDITIONAL_ARGUMENT_COUNT 4

	char** const array = (char**)malloc(sizeof(char*) * (argc + ADDITIONAL_ARGUMENT_COUNT));

	if(array == NULL) {
		fprintf(stderr, "OOM\n");
		return 1;
	}

	array[0] = NODE_PATH;
	array[1] = EXEC_DIR;
	array[2] = SCRIPT_VERSION;
	array[3] = GIT_COMMIT_HASH;
	for(size_t i = 1; i < (size_t)argc; ++i) {
		array[i + ADDITIONAL_ARGUMENT_COUNT - 1] = argv[i];
	}

	array[argc + ADDITIONAL_ARGUMENT_COUNT - 1] = NULL;

	execv(NODE_PATH, array);

	fprintf(stderr, "rapper script failed with: %s\n", strerror(errno));
	return 1;
}
