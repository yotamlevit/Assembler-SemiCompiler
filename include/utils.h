//
// Created by Yotam Levit on 19/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H

#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FILE* open_file(char* file);

int is_operation(char*);

int is_stop(char*);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
