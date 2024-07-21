//
// Created by Yotam Levit on 19/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H


#include <stdio.h>

#define STOP_LENGTH 4
#define STOP_OPCODE 15
#define OPERATION_LENGTH 3

FILE* open_file(char* file);

int is_operation(char*);

int is_stop(char*);


char* find_next_symbol_in_line(char* line, char symbol);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
