#include "json.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE (1)
#define FALSE (0)
#define MAX_STRLEN (256)

/* starts of json parts */
int is_obj_start(char c) { return c == '{'; }

int is_array_start(char c) { return c == '['; }

int is_number_start(char c) { return c == '-' || isdigit(c); }

int is_string_start(char c) { return c == '"' || c == '\''; }

int is_white_space(char c) { return isspace(c); }

/**
 * Load json object
 * @param obj object on same level to connect new object to
 * @param file data input stream
 * @param int 1 if it's needed to find first character, 0 if it was previous cha
 */
c_json *json_load_obj(c_json *obj, FILE *file);

/**
 * Simple function to consume all white spaces
 * @param file data input stream
 * @return next char after whitespaces
 */
char json_load_whitespace(FILE *file);

char *json_load_string(FILE *file);

c_json *json_load(char *file_name) {
	FILE *file = fopen(file_name, "r");

	/* file not found */
	if (!file) {
		printf("Error: file %s not found.\n", file_name);
		return NULL;
	}

	char c;

	/* create root object */
	if (json_load_whitespace(file) != '{') {
		printf("Error: missing root object.\n");
		return NULL;
	}

	c_json *root_obj = json_load_obj(NULL, file);
	return root_obj;
}

char json_load_whitespace(FILE *file) {
	if (!file) return 0;
	char c;
	while ((c = fgetc(file)) != EOF) {
		if (!isspace(c)) {
			ungetc(c, file);
			return c;
		}
	}
	return 0;
}

char *json_load_string(FILE *file) {
	char buffer[MAX_STRLEN];
	char *b = buffer, c;

	if ( (c = fgetc(file)) != '"' && c != '\'') {
		printf("Error: missing leading '\"' in string, found: '%c'\n", c);
	}

	while ( (b - buffer) < MAX_STRLEN - 1 && (c = fgetc(file)) != '"' && c != '\'') {
		*b = c;
		b++;
	}
	*b = '\0';

	char *new_str = strdup(buffer);
	return new_str;
}

c_json *json_load_obj(c_json *obj, FILE *file) {
	c_json *new_obj = calloc(1, sizeof(c_json));
	char c;

	if (obj) {
		obj->next = new_obj;
	}
	obj = new_obj;

	if ((c = fgetc(file)) != '{') {
		printf("Error: missing leading '{' in object, found: '%c'\n", c);
	}

	if (json_load_whitespace(file) == '}') {
		// empty object
		return new_obj;
	}
	// load key value
	obj->key = json_load_string(file);
	printf("object '%s'\n", obj->key);

	// read ':'
	if ((c = json_load_whitespace(file)) != ':') {
		printf("Error: missing ':' between key and value, found: '%c'\n", c);
		free(obj);
		return NULL;
	}

	json_load_whitespace(file);
	// load value

	return obj;
}
