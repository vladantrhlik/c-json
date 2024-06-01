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
} c_json_valtype;

typedef struct the_c_json_value{
	c_json_valtype type;

	union value {
		float number;
		char *string;
		struct the_c_json *object;
		struct the_c_json_value *array;
	};

	int is_array;
	struct the_c_json_value *next;

} c_json_value;

typedef struct the_c_json {
	struct the_c_json *next, *prev;
	char *key;
	c_json_value value;
} c_json;

c_json *json_load(char *file_name);

void json_print(c_json *obj, int indent);

#endif
