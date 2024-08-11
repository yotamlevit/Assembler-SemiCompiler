
#ifndef ASSEMBLER_SEMICOMPILER_FIRST_PASS_H
#define ASSEMBLER_SEMICOMPILER_FIRST_PASS_H

#include <stdio.h>

#include "globals.h"
#include "hash_map.h"

#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

boolean immediate_address(char* li, char* addressing_mode);
boolean indirect_register_address(char* li, char* addressing_mode);
boolean direct_register_address(char* li, char* addressing_mode);
boolean direct_address(char* addressing_mode);
boolean get_addressing_mode(char*, char* addressing_mode);
boolean analyze_input_line(char*, HashMapPtr macro_map);
void fix_symbol_addresses();
boolean first_pass_exec(FILE * file_handle, HashMapPtr macro_map);
boolean is_label(char*);
boolean label_actions(char*, HashMapPtr macro_map);
boolean handle_coma(char* asm_line);
boolean get_src_and_dst_operands(char* asm_line, char* operand_src, char* operand_dst);
boolean validate_opcode_with_operands(char operand_src, char operand_dst);
boolean operation(char*);
boolean ext(char*);
boolean insert_numerical_data(char*);
boolean insert_string_data(char*);

#endif