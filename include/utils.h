//
// Created by Yotam Levit on 19/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H


#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STOP_LENGTH 4
#define STOP_OPCODE 15
#define OPERATION_LENGTH 3
#define END_OF_STR '\0'

/* File functions */
FILE* open_file(char* file, char* mode);
boolean write_line_to_file(FILE* fp, char* line);

/* Get operation Functions */
int is_operation(char* asm_line);
int is_stop(char*);
boolean is_register(char* str);

/* Parse ASM line */
char* find_next_symbol_in_str(char* line, char symbol);

/*Auxiliary functions*/
char* delete_first_spaces(char*);

void add_extension_2_file_name(char*);
void add_file_name_extension(char* filename,char* extension);

void clean_line(char*);
void clean_label_name(char*);
boolean ends_with_newline(const char *str);
boolean is_label(char* asm_line);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
