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
FILE* open_file(char* file);

/* Get operation Functions */
int is_operation(char* li);
int is_stop(char*);

/* Parse ASM line */
char* find_next_symbol_in_str(char* line, char symbol);

/*Auxiliary functions*/
char* delete_first_spaces(char*);

void add_extension_2_file_name(char*);

void clean_line(char*);

void clean_label_name(char*);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
