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
 */
c_json *json_load_obj(c_json *prev, FILE *file);

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
 * @param file data input stream
 */
float json_load_number(FILE *file);

/**
 * Read value (string/number/array/object/true/false/null) and store it in c_json object
 * @param value to store value in
 * @param file data input stream
 */
void json_load_value(c_json_value *value, FILE *file);

/**
 * Load array or values
 * @param value entry points of this array
 * @param file data input stream
 */
void json_load_array(c_json_value *value, FILE *file);


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

float json_load_number(FILE *file) {
	if (!file) return -1;

	char buffer[MAX_STRLEN];
	char *b = buffer;
	char c;

	// read whole number
	while ( (c = fgetc(file)) != EOF && (isdigit(c) || c == '+' || c == '-' || c == '.')) {
		*b = c;
		b++;
	}
	ungetc(c, file);
	*b = '\0';

	float res = atof(buffer);
	return res;
}

void json_load_array(c_json_value *value, FILE *file) {
	if (!value || !file) return;

	// create start of linked list
	value->array = calloc(1, sizeof(c_json_value));
	if (!value->array) return;
	value = value->array;

	char c = fgetc(file);
	if (c != '[') {
		printf("Error: missing leading '[' when loading array");
	}

	while ( (c = json_load_whitespace(file)) != EOF && c != ']') {

		if (c == ',') {
			fgetc(file);
			// next value
			c_json_value *next_value = calloc(1, sizeof(c_json_value));
			if (!next_value) return;
			next_value->is_array = 1;

			value->next = next_value;
			value = next_value;
			continue;
		}

		json_load_value(value, file);
	}
	getc(file); // remove ']'
	return;
}

void json_load_value(c_json_value *value, FILE *file) {
	if (!value || !file) return;

	char c = fgetc(file);
	ungetc(c, file);

	switch (c) {
		case '{':
			value->type = OBJECT;
			value->object = json_load_obj(NULL, file);
			return;
		case '"':
		case '\'':
			value->type = STRING;
			value->string = json_load_string(file);
			return;
		case '[':
			value->is_array = 1;
			value->type = ARRAY;
			json_load_array(value, file);
			return;
	}

	// check if number
	if (isdigit(c) || c == '+' || c == '-') {
		value->type = NUMBER;
		value->number = json_load_number(file);
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

	if (!strcmp(buffer, "true")) value->type = TRUE;
	if (!strcmp(buffer, "false")) value->type = FALSE;
	if (!strcmp(buffer, "null")) value->type = NONE;
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
		json_load_value(&obj->value, file);
	}

	return root_obj;
}

void json_print_value(c_json_value *value, int indent) {
	switch (value->type) {
		case STRING:
			printf("\"%s\"", value->string);
			break;
		case NUMBER:
			printf("%f", value->number);
			break;
		case ARRAY:
			printf("[");
			value = value->array;
			while (value) {
				json_print_value(value, indent);
				if (value->next) printf(", ");
				value = value->next;
			}
			printf("]");
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
			json_print(value->object, indent + 1);
			for (int i = 0; i < indent; i++) printf("\t");
			printf("}");
			break;
	}
}

void json_print(c_json *obj, int indent) {
	if (!obj) return;

	for (int i = 0; i < indent; i++) printf("\t");

	printf("'%s': ", obj->key);
	json_print_value(&obj->value, indent);
	printf("\n");

	if (obj->next) {
		json_print(obj->next, indent);
	}
}

c_json *json_find(c_json *json, char *key) {
	if (!json) return NULL;

	c_json *start = json;

	// look in next keys
	while (json) {
		if (!strcmp(key, json->key)) return json;
		json = json->next;
	}

	// look in prev keys
	json = start->prev;

	while (json) {
		if (!strcmp(key, json->key)) return json;
		json = json->prev;
	}
	return NULL;
}

c_json_iter *json_new_iter(c_json *json) {
	if (!json) return NULL;
	// traverse to start
	while (json->prev) json = json->prev;
	// create copy of first c_json
	c_json_iter *iter = calloc(1, sizeof(c_json_iter));
	if (!iter) return NULL;
	memcpy(iter, json, sizeof(c_json));
	return iter;
}

c_json *json_iter_next(c_json_iter **iter) {
	if (!iter || !*iter) return NULL;
	*iter = (*iter)->next;
	return *((c_json**)iter);
}

c_json *json_iter_get(c_json_iter *iter) {
	if (!iter) return NULL;
	return (c_json*)iter;
}

