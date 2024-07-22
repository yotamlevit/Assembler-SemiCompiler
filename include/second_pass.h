/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_SECOND_PASS_H
#define ASSEMBLER_SEMICOMPILER_SECOND_PASS_H

#include <stdio.h>
#include "auxiliary.h"

boolean process_line(char* asm_line, code_word_fields_ptr code_word);
boolean second_pass_exec(FILE * file_handle);
boolean isLabel2(char*);
boolean process_entry(char*);
boolean process_label(char* asm_line, code_word_fields_ptr code_word);
boolean second_operation(char* asm_line, code_word_fields_ptr code_word);
symbol* find_symbol(const char* asm_line, int symbol_name_length);

#endif
