
#ifndef ASSEMBLER_SEMICOMPILER_FIRST_PASS_H
#define ASSEMBLER_SEMICOMPILER_FIRST_PASS_H

#include <stdio.h>

#include "globals.h"
#include "hash_map.h"

#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

boolean immediate_address(char* li, char* addressing_mode, int* line_index);
boolean indirect_register_address(char* li, char* addressing_mode, int* line_index);
boolean direct_register_address(char* li, char* addressing_mode, int* line_index);
boolean direct_address(char* addressing_mode);
boolean get_addressing_mode(char*, char* addressing_mode, int* line_index);
boolean analyze_input_line(char*, HashMapPtr macro_map, int* line_index, int* opcode);
boolean first_pass_exec(FILE * file_handle, HashMapPtr macro_map, int* line_index);
boolean label_actions(char*, HashMapPtr macro_map, int* line_index, int* opcode);
boolean handle_coma(char* asm_line, int* line_index);
boolean get_src_and_dst_operands(char* asm_line, char* operand_src, char* operand_dst, int* line_index);
boolean validate_opcode_with_operands(char operand_src, char operand_dst, int* line_index, int* opcode);
boolean handle_no_operands(int* opcode);
boolean operation(char*, int* line_index, int* opcode);
boolean ext(char*, int* line_index);
boolean insert_numerical_data(char*, int* line_index);
boolean insert_string_data(char*, int* line_index);
void fix_symbol_addresses();

#endif
