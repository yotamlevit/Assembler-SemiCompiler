
#include <stdio.h>
#include "globals.h"
#include "status_codes.h"


#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

//extern char line[MAX_LINE_LENGTH];


/*Functions*/
char addressing_mode(char*);

/*Two transition functions*/
boolean analize_input_line(char*);

void fix_symbol_addresses();

int first_pass_exec(FILE * file_handle);

int is_label(char*);

void label_actions(char*);

void operation(char*);

boolean ext(char*);

boolean insert_numerical_data(char*);

void insert_string_data(char*);