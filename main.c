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

	json_print(json, 0);
	
	/*
	c_json *players = json_find(json, "players");
	printf("players %s found!\n", players ? "" : "not");
	*/

	// print all keys with iterator
	printf("all found keys:\n");
	for (c_json_iter *i = json_new_iter(json); json_iter_get(i); json_iter_next(&i)) {
		printf("\'%s\'\n", json_iter_get(i)->key);
	}
}
