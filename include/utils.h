//
// Created by Yotam Levit on 19/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H


#include <stdio.h>

#define STOP_LENGTH 4
#define STOP_OPCODE 15
#define OPERATION_LENGTH 3
#define END_OF_STR '\0'

/* File functions */
FILE* open_file(char* file);

/* Get operation Functions */
int is_operation(char* li, int* opcode);
int is_stop(char*, int* opcode);

/* Parse ASM line */
char* find_next_symbol_in_line(char* line, char symbol);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
