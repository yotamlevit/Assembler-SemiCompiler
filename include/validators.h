/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_VALIDATORS_H
#define ASSEMBLER_SEMICOMPILER_VALIDATORS_H


#define MEMORY_SIZE 4096

int validate_input(int argc, char** argv);

int validate_memory(int IC, int DC);

int validate_second_pass();

#endif
