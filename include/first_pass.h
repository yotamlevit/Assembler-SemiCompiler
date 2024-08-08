
#include <stdio.h>
#include "globals.h"
#include "hash_map.h"


#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

//extern char line[MAX_LINE_LENGTH];


/*Functions*/
char addressing_mode(char*);

/*Two transition functions*/
boolean analyze_input_line(char*, HashMapPtr macro_map);

void fix_symbol_addresses();

boolean first_pass_exec(FILE * file_handle, HashMapPtr macro_map);

boolean is_label(char*);

boolean label_actions(char*, HashMapPtr macro_map);

boolean operation(char*);

boolean ext(char*);

boolean insert_numerical_data(char*);

boolean insert_string_data(char*);