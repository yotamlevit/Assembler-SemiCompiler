
#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H

#include "globals.h"
#include <stdio.h>

#define STOP_LENGTH 4
#define STOP_OPCODE 15
#define OPERATION_LENGTH 3
#define END_OF_STR '\0'

FILE* open_file(char* file, char* mode);
boolean write_line_to_file(FILE* fp, char* line);
int is_operation(char* asm_line);
int is_stop(char*);
boolean is_register(char* str);
char* find_next_symbol_in_str(char* line, char symbol);
char* delete_first_spaces(char*);
void add_file_name_extension(char* filename,char* extension);
void clean_line(char*);
void clean_label_name(char*);
boolean ends_with_newline(const char *str);
boolean is_label(char* asm_line);

#endif
