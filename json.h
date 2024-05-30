#ifndef C_JSON
#define C_JSON

typedef struct the_c_json {
	struct the_c_json *next, *prev;
	char *key;
	union value {
		float number;
		char *string;
		struct the_c_json *array;
	};
} c_json;

c_json *json_load(char *file_name);

#endif
