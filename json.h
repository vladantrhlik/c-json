#ifndef C_JSON
#define C_JSON

typedef enum {
	NONE,
	NUMBER,
	STRING,
	ARRAY,
	OBJECT,
	TRUE,
	FALSE
} c_json_val;

typedef struct the_c_json {
	struct the_c_json *next, *prev;
	char *key;
	c_json_val value_type;
	union value {
		float number;
		char *string;
		struct the_c_json **array;
		struct the_c_json *object;
	};
} c_json;

c_json *json_load(char *file_name);

#endif
