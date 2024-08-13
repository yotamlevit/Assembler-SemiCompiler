
#ifndef PREPROCESS_H
#define PREPROCESS_H


#include <stdio.h>
#include "globals.h"
#include "hash_map.h"

#define MACRO_OUTPUT_EXTENSION "asm" /* Extension for the output file after processing macros. */

boolean is_macro_definition(char* pos);
int count_macro_occurrences(FILE* file);
HashMapPtr init_macro_hash_map(FILE* file);
void *handle_malloc(long object_size);
boolean is_valid_macro_definition(char **name, int line_count);
char *copy_macro_content(FILE *fp, int length);
char *save_macro_content(FILE *fp, fpos_t *pos, int *line_count);
boolean add_macro_to_map(FILE* file, HashMapPtr macro_map, char* macro_name, int line_count);
boolean handle_new_macro(FILE* file, HashMapPtr macro_map, char* macro_name, int line_count);
void handle_non_new_macro_line(char* filename, char* pos, char* original_line, HashMapPtr macro_map, int line_count);
boolean process_macro_file(FILE* file, HashMapPtr macro_map, char* asm_filename);
int macro_exec(FILE* file, char* filename, HashMapPtr* macro_map);

#endif
