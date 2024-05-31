#include "json.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRLEN (256)

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

/**
 * Read string between "" or ''
 * @param file data input stream
 */
char *json_load_string(FILE *file);

/**
 * Read number (int / float) and save it into obj
 * @param obj object to save value to
 * @param file data input stream
 */
float json_load_number(c_json *obj, FILE *file);

/**
 * Read value (string/number/array/object/true/false/null) and store it in c_json object
 * @param obj object to store value in
 * @param file data input stream
 */
void json_load_value(c_json *obj, FILE *file);

void json_load_array(FILE *file);


c_json *json_load(char *file_name) {
	FILE *file = fopen(file_name, "r");

	/* file not found */
	if (!file) {
		printf("Error: file %s not found.\n", file_name);
		return NULL;
	}

	/* create root object */
	if (json_load_whitespace(file) != '{') {
		ungetc('{', file);
		printf("Warning: missing root object.\n");
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

float json_load_number(c_json *obj, FILE *file) {
	if (!obj || !file) return -1;

	char buffer[MAX_STRLEN];
	char *b = buffer;
	char c;

	// read whole number
	while ( (c = fgetc(file)) != EOF && (isdigit(c) || c == '+' || c == '-' || c == '.')) {
		*b = c;
		b++;
	}
	*b = '\0';

	float res = atof(buffer);
	obj->value.number = res;

	return res;
}

void json_load_array(FILE *file) {
	if (!file) return;
	char c;
	while ( (c = fgetc(file)) != EOF && c != ']') {}
	return;
}

void json_load_value(c_json *obj, FILE *file) {
	if (!obj || !file) return;

	char c = fgetc(file);
	ungetc(c, file);

	switch (c) {
		case '{':
			obj->value.type = OBJECT;
			obj->value.object = json_load_obj(NULL, file);
			return;
		case '"':
		case '\'':
			obj->value.type = STRING;
			obj->value.string = json_load_string(file);
			return;
		case '[':
			obj->value.type = ARRAY;
			printf("arrays not implemented\n");
			json_load_array(file);
			return;
	}

	// check if number
	if (isdigit(c) || c == '+' || c == '-') {
		obj->value.type = NUMBER;
		json_load_number(obj, file);
		return;
	}

	// check true, false, null
	// read until non-alpha char
	char buffer[MAX_STRLEN];
	char *b = buffer;
	while ((c = fgetc(file)) != EOF && b - buffer < MAX_STRLEN) {
		if (!isalpha(c)) {
			ungetc(c, file);
			break;
		}
		*b = c;
		b++;
	}
	*b = '\0';

	printf("special key value: %s\n", buffer);
}

c_json *json_load_obj(c_json *obj, FILE *file) {
	c_json *root_obj, *new_obj = calloc(1, sizeof(c_json));
	char c;

	if (obj) {
		obj->next = new_obj;
	}
	obj = new_obj;
	root_obj = obj;

	if ((c = fgetc(file)) != '{') {
		printf("Error: missing leading '{' in object, found: '%c'\n", c);
	}

	while (1) {
		if ((c = json_load_whitespace(file)) == '}' || c == EOF) {
			fgetc(file); // remove '}'
			return root_obj;
		} else if (c == ',') {
			printf("next obj\n");
			fgetc(file);
			json_load_whitespace(file);
			// create new object and connect to last
			new_obj = calloc(1, sizeof(c_json));
			if (!new_obj) {
				return root_obj;
			}
			obj->next = new_obj;
			obj = new_obj;
		}

		// load key value
		obj->key = json_load_string(file);

		// read ':'
		if ((c = json_load_whitespace(file)) != ':') {
			printf("Error: missing ':' between key and value, found: '%c'\n", c);
			free(obj);
			return root_obj;
		}
		fgetc(file); // remove ':'
		json_load_whitespace(file); 

		// load value
		json_load_value(obj, file);

		// print for debug
	}

	return root_obj;
}

void json_print(c_json *obj, int indent) {
	if (!obj) return;

	for (int i = 0; i < indent; i++) printf("\t");

	printf("'%s': ", obj->key);
	switch (obj->value.type) {
		case STRING:
			printf("%s", obj->value.string);
			break;
		case NUMBER:
			printf("%f", obj->value.number);
			break;
		case ARRAY:
			printf("[]");
			break;
		case NONE:
			printf("null");
			break;
		case TRUE:
			printf("true");
			break;
		case FALSE:
			printf("false");
			break;
		case OBJECT:
			printf("{\n");
			json_print(obj->value.object, indent + 1);
			for (int i = 0; i < indent; i++) printf("\t");
			printf("}");
			break;
	}
	printf("\n");
	if (obj->next) {
		json_print(obj->next, indent);
	}
}
