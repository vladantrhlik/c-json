#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Error: missing file\n");
		return EXIT_FAILURE;
	}

	char *file_name = argv[1];
	c_json *json = json_load(file_name);

	return EXIT_SUCCESS;
}

