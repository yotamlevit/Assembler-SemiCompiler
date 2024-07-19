
#include <stdio.h>
#include "globals.h"


#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

//extern char line[MAX_LINE_LENGTH];


/*Functions*/
char addressing_mode(char*);

/*Two transition functions*/
void analize_input_line(char*);

void fix_symbol_addresses();

int first_pass_exec(FILE * file_handle);
