#include "json.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* starts of json parts */
int is_obj_start(char c) { return c == '{'; }

int is_array_start(char c) { return c == '['; }

int is_number_start(char c) { return c == '-' || isdigit(c); }

int is_string_start(char c) { return c == '"' || c == '\''; }

int is_white_space(char c) { return isspace(c); }


void json_load_obj(c_json *obj, FILE *file);

c_json *json_load(char *file_name) {
	FILE *file = fopen(file_name, "r");

	/* file not found */
	if (!file) {
		printf("Error: file %s not found.\n", file_name);
		return NULL;
	}

	char c;

	/* create root object */
	c_json *curr_obj = NULL, *root_obj = NULL;

	/* load data */
	while ((c = fgetc(file)) != EOF) {
		if (c == '{') {
			/* generate new object structure */
			if (curr_obj) {
				curr_obj->next = calloc(1, sizeof(c_json));
				curr_obj->next->prev = curr_obj;
				curr_obj = curr_obj->next;
			} else {
				curr_obj = calloc(1, sizeof(c_json));
				root_obj = curr_obj;
			}

			if (!curr_obj) return NULL;
			json_load_obj(curr_obj, file);
		} else if (!isspace(c)) {
			printf("Warning: unexpected character: %c\n", c);
		}
	}

	return root_obj;
}

void json_load_obj(c_json *obj, FILE *file) {
	if (!obj) return;

	char c;
	while ((c = fgetc(file)) != EOF) {
		if (c == '}') return;
		else if (c == '"' || c == '\'') {

		}
	}

}
