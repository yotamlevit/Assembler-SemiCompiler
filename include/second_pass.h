/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_SECOND_PASS_H
#define ASSEMBLER_SEMICOMPILER_SECOND_PASS_H

#include <stdio.h>
#include "auxiliary.h"


boolean analize_2_second_pass(char* asm_line, code_word_fields_ptr code_word);

int second_pass_exec(FILE * file_handle);

int isLabel2(char*);

boolean process_entry(char*);

boolean process_lable(char* asm_line, code_word_fields_ptr code_word);

boolean second_operation(char* asm_line, code_word_fields_ptr code_word);


void handle_error(const char* message);

int find_symbol(const char* li, int length);

#endif
