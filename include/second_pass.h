
#ifndef ASSEMBLER_SEMICOMPILER_SECOND_PASS_H
#define ASSEMBLER_SEMICOMPILER_SECOND_PASS_H

#include <stdio.h>
#include "tables.h"
#include "globals.h"

int get_symbol_length(const char* asm_line);
symbol_ptr find_symbol(const char* asm_line, int symbol_name_length);
void update_machine_word(machine_word_fields_ptr machine_word, int w, int A, boolean is_src);
int get_operand_offset_value(code_word_fields_ptr code_word, boolean is_src);
boolean handle_operand(char* asm_line, code_word_fields_ptr code_word, machine_word_fields_ptr dest_machine_word, boolean is_src);
boolean handle_one_operand(char* asm_line, code_word_fields_ptr code_word);
boolean handle_registers_method(char* asm_line, machine_word_fields_ptr machine_word);
boolean handle_two_operands_method(char* asm_line, code_word_fields_ptr code_word);
boolean is_registry_method(code_word_fields_ptr code_word);
boolean handle_two_operands(char* asm_line, code_word_fields_ptr code_word);
boolean second_operation(char* asm_line, code_word_fields_ptr code_word, int* line_index, int* opcode);
boolean process_label(char* asm_line, code_word_fields_ptr code_word, int* line_index);
boolean process_entry(char* asm_line);
boolean process_line(char* asm_line, code_word_fields_ptr code_word, int* line_index);
boolean second_pass_exec(FILE* file_handle, int* line_index);

#endif
