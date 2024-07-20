//
// Created by Yotam Levit on 19/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_UTILS_H
#define ASSEMBLER_SEMICOMPILER_UTILS_H

#include "../include/utils.h"
#include "../include/second_pass.h"
#include "../include/validators.h"
#include "../include/auxiliary.h"
#include "../include/first_pass.h"
#include "../include/constants_tables.h"
#include "../include/globals.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STOP_LENGTH 4
#define STOP_OPCODE 15
#define OPERATION_LENGTH 3

FILE* open_file(char* file);

int is_operation(char*);

int is_stop(char*);

#endif //ASSEMBLER_SEMICOMPILER_UTILS_H
