//
// Created by Yotam Levit on 18/07/2024.
//

#ifndef ASSEMBLER_SEMICOMPILER_VALIDATORS_H
#define ASSEMBLER_SEMICOMPILER_VALIDATORS_H

#include "assembler.h"

int validate_input(int argc, char** argv);

int validate_memory(int IC, int DC);

int validate_second_pass();

#endif //ASSEMBLER_SEMICOMPILER_VALIDATORS_H
