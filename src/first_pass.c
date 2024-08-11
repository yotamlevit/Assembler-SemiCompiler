
#include <stdio.h>
#include <string.h>

#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/tables.h"
#include "../include/first_pass.h"
#include "../include/hash_map.h"
#include "../include/constants_tables.h"
#include "../include/globals.h"
#include "../include/logger.h"

int opcode;

/**
 * @brief Analyzes and processes an assembly line.
 *
 * The analyze_input_line function processes a given assembly line to determine its type
 * (directive, instruction, comment, or label) and performs the corresponding actions.
 * It handles external labels, numerical data, string data, entry labels, and operations.
 * If the line contains an unrecognized command, an error is logged.
 *
 * @param asm_line A pointer to the assembly line to be analyzed and processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the line is processed successfully. Otherwise, returns FALSE.
 */
boolean analyze_input_line(char* asm_line, HashMapPtr macro_map, int* line_index)
{
	asm_line = delete_first_spaces(asm_line);
	if (!strncmp(asm_line, EXTERN_LABEL, strlen(EXTERN_LABEL)))
		return ext(asm_line + strlen(EXTERN_LABEL), line_index);
	if (!strncmp(asm_line, DATA_LABEL, strlen(DATA_LABEL)))
		return insert_numerical_data(asm_line + strlen(DATA_LABEL), line_index);
	if (!strncmp(asm_line, STRING_LABEL, strlen(STRING_LABEL)))
		return insert_string_data(asm_line + strlen(STRING_LABEL), line_index);
	if(*asm_line == ';' || *asm_line == '\0' || *asm_line == '\n' || (!strncmp(asm_line, ENTRY_LABEL, strlen(ENTRY_LABEL))))
		return TRUE;
	opcode = is_operation(asm_line);
    if (opcode != -1)
		return operation(asm_line + 3, line_index);
	opcode = is_stop(asm_line);
	if (opcode != -1)
		return operation(asm_line + 4, line_index);
	if (is_label(asm_line))
		return label_actions(asm_line, macro_map, line_index);

	error_log("line %d: The command was not found\n", *line_index);
	return FALSE;
}


/**
 * @brief Checks and sets the addressing mode for an immediate address.
 *
 * This function checks if the given operand is a valid immediate address (prefixed with `#`).
 * It validates that the characters following the `#` are digits, or optionally a `+` or `-` sign,
 * followed by digits. If the operand is valid, the addressing mode is set to `'0'`.
 *
 * @param li A pointer to the operand string.
 * @param addressing_mode A pointer to a character where the addressing mode will be stored.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the immediate address is valid.
 *         Returns TRUE if valid, FALSE otherwise.
 */
boolean immediate_address(char* li, char* addressing_mode, int* line_index) {
	int i = 2;
	boolean result = TRUE;
	if (li[1] < 47 || li[1]>58)
	{
		if (li[1] != '-' && li[1] != '+')
		{
			error_log("line %d: Invalid parameter for the instant address\n", *line_index);
			result = FALSE;
		}
	}
	else
	{
		while (li[i] != ' ' && li[i] != ',' && li[i] != '\0' && li[i] != '\n' && li[i] != '\t')
		{
			/*If after # does not appear a number - throw an error*/
			if (li[i] < 47 || li[i]>58)
			{
				error_log("line %d: Invalid parameter for the instant address\n", *line_index);
				result = FALSE;
				break;
			}
			i++;
		}
	}
	*addressing_mode = '0';
	return result;
}


/**
 * @brief Checks and sets the addressing mode for an indirect register address.
 *
 * This function checks if the given operand is a valid indirect register address (prefixed with `*r`).
 * It validates that the register number is between 0 and 7. If the operand is valid, the addressing mode is set to `'2'`.
 *
 * @param li A pointer to the operand string.
 * @param addressing_mode A pointer to a character where the addressing mode will be stored.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the indirect register address is valid.
 *         Returns TRUE if valid, FALSE otherwise.
 */
boolean indirect_register_address(char* li, char* addressing_mode, int* line_index) {
	boolean result = TRUE;
	if (!(*(li + 1) == 'r'))
	{
		error_log("line %d: Invalid override parameter\n", *line_index);
		result = FALSE;
	}
	/*If the register is not between 0-7*/
	if (!(*(li + 2) > 47 && *(li + 2) < 56))
	{
		error_log("line %d: Invalid indirect address registration\n", *line_index);
		result = FALSE;
	}
	*addressing_mode = '2';
	return result;
}


/**
 * @brief Checks and sets the addressing mode for a direct register address.
 *
 * This function checks if the given operand is a valid direct register address (e.g., `r0`, `r1`, etc.).
 * It validates that the register number is between 0 and 7. If the operand is valid, the addressing mode is set to `'3'`.
 *
 * @param li A pointer to the operand string.
 * @param addressing_mode A pointer to a character where the addressing mode will be stored.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the direct register address is valid.
 *         Returns TRUE if valid, FALSE otherwise.
 */
boolean direct_register_address(char* li, char* addressing_mode, int* line_index) {
	boolean result = TRUE;
	/*If the register is not between 0-7*/
	if (!(*(li + 1) > 47 && *(li + 1) < 56))
	{
		error_log("line %d: Invalid direct address registration\n", *line_index);
		result = FALSE;
	}
	*addressing_mode = '3';
	return result;
}


/**
 * @brief Sets the addressing mode for a direct address.
 *
 * This function simply sets the addressing mode to `'1'` for a direct address.
 * A direct address is usually a label or a memory address.
 *
 * @param addressing_mode A pointer to a character where the addressing mode will be stored.
 * @return A boolean value indicating success.
 *         Always returns TRUE.
 */
boolean direct_address(char* addressing_mode) {
	*addressing_mode = '1';
	return TRUE;
}


/**
 * @brief Determines and sets the addressing mode for the given operand.
 *
 * This function analyzes the operand string and determines its addressing mode.
 * It delegates the specific checks to other functions depending on the operand's prefix
 * (`#` for immediate, `*` for indirect register, `r` for direct register, or anything else for direct address).
 *
 * @param li A pointer to the operand string.
 * @param addressing_mode A pointer to a character where the addressing mode will be stored.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether a valid addressing mode was found.
 *         Returns TRUE if a valid addressing mode is found, FALSE otherwise.
 */
boolean get_addressing_mode(char* li, char* addressing_mode, int* line_index)
{
	li = delete_first_spaces(li);
	if (*li == '#')
		return immediate_address(li, addressing_mode, line_index);
	if (*li == '*')
		return indirect_register_address(li, addressing_mode, line_index);
	if (*li == 'r')
		return direct_register_address(li, addressing_mode, line_index);
	if (*li > 20 && *li < 127)
		return direct_address(addressing_mode);

	*addressing_mode = ' ';
	return TRUE;
}


/**
 * @brief Adjusts the addresses of symbols in the symbol table.
 *
 * The fix_symbol_addresses function traverses the symbol table and adjusts the addresses
 * of symbols that are attached to directives. It increments their addresses by the
 * current value of the instruction counter (IC) to reflect their final positions in memory.
 *
 * @return void This function does not return a value.
 */
void fix_symbol_addresses()
{
	symbol* temp = head_symbol;
	while (temp)
	{
		if (temp->is_attached_directive)
		{
			temp->address += IC;
		}
		temp = temp->next;
	}
}


/**
 * @brief Executes the first pass of the assembler process on the input file.
 *
 * The first_pass_exec function reads each line from the given input file, processes it
 * using the analyze_input_line function, and increments the line counter. It also validates
 * memory usage by checking the instruction counter (IC) and data counter (DC) at the end
 * of the pass.
 *
 * @param file_handle A pointer to the file to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the first pass.
 *         Returns TRUE if the first pass is executed successfully. Otherwise, returns FALSE.
 */
boolean first_pass_exec(FILE* file_handle, HashMapPtr macro_map, int* line_index)
{
	boolean result = TRUE, analyze_input_line_result;
	*line_index = 0;
    while (!feof(file_handle))
    {
        analyze_input_line_result = analyze_input_line(line, macro_map, line_index);
    	if (!analyze_input_line_result)
    		result = FALSE;
        *line_index = *line_index + 1;
        fgets(line, MAX_LINE_LENGTH, file_handle);
    }
    validate_memory(IC, DC);
	return result;
}


/**
 * @brief Processes label-related actions in an assembly line.
 *
 * The label_actions function processes a given assembly line to handle labels.
 * It checks for the presence of a label (indicated by a colon ':'), validates the label length,
 * allocates memory for a new symbol, and updates the symbol table. Depending on the type of
 * statement following the label (directive or instruction), the function delegates further
 * processing to the appropriate handler.
 *
 * @param asm_line A pointer to the assembly line to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the label is processed successfully. Otherwise, returns FALSE.
 */
boolean label_actions(char* asm_line, HashMapPtr macro_map, int* line_index)
{
	char* p, * label_name;
	symbol* temp;
	int i;
	boolean result = TRUE;
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		if (asm_line[i] == ':')
		{
			label_name = (char*)malloc((i + 1) * sizeof(char));
			strncpy(label_name, asm_line, i);
			if ((hashMapFind(macro_map, label_name)) != NULL) {
				error_log("line %d: A label cannot be a macro name\n" ,*line_index);
				free(label_name);
				result = FALSE;
			}
			if (i > MAX_LABEL_LENGTH)
			{
				error_log("line %d: Lable is too long, has more than 30 chars\n", *line_index);
				return FALSE;
			}
			temp = (symbol*)malloc(sizeof(symbol));
			if (temp == NULL)
			{
				error_log("Memory allocation error");
				return FALSE;
			}
			temp->next = head_symbol;
			head_symbol = temp;
			clean_label_name(head_symbol->symbol_name);
			strncpy(head_symbol->symbol_name, asm_line, i);
			p = (asm_line + i + 1);
			p = delete_first_spaces(p);
			if (*p == '.')
			{
				/*If its guide statement*/
				head_symbol->address = DC;
				head_symbol->is_attached_directive = TRUE;
				head_symbol->is_external = FALSE;
				if (!strncmp(p, ENTRY_LABEL, strlen(ENTRY_LABEL)))
					warning_log("line %d: A label defined at the beginig of entry statement is ignored\n" ,*line_index);
				else if (!strncmp(p, EXTERN_LABEL, strlen(EXTERN_LABEL)))
					warning_log("line %d: A label defined at the beginig of extern statement is ignored.\n", *line_index);
				else
					/*Sends again to analize to find out if its string or data*/
					return analyze_input_line(p, macro_map, line_index);
			}
			else
			{
				/*If this is a statement of instruction*/
				head_symbol->address = IC;
				head_symbol->is_attached_directive = FALSE;
				head_symbol->is_external = FALSE;
				/*Go again to analize to find out which instruction statement*/
				return analyze_input_line(p, macro_map, line_index);
			}
		}
	}
	return result;
}


/**
 * @brief Handles and validates comma placement in an assembly line.
 *
 * This function checks the placement of commas within an assembly instruction line. It ensures
 * that commas are correctly used to separate operands and that no invalid characters are
 * present between operands. If a comma is missing where expected, it logs an error and
 * attempts to correct the line by inserting a missing comma.
 *
 * @param asm_line A pointer to the assembly instruction line to be checked.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the comma placement is valid.
 *         Returns TRUE if commas are correctly placed, FALSE if there is an issue.
 */
boolean handle_coma(char* asm_line, int* line_index) {
	boolean result = TRUE, miss_comma = FALSE;
	int s, k;
	char oper[MAX_LINE_LENGTH - 4];
	char* p = oper;

	for (k = 0; asm_line[k] == ' ' || asm_line[k] == '\t'; k++);
	strcpy(oper, asm_line + k);

	for (k = 0; p[k] != '\0' && p[k] != '\n'; k++)
	{
		if (p[k] == ' ')
		{
			while (p[k] == ' ' || p[k] == '\t')
				k++;
			if (p[k] == ',')
			{
				k++;
				while (p[k] == ' ' || p[k] == '\t')
					k++;
				if (p[k] == '\0')
					break;
			}
			else if (p[k] != '\0' && p[k] != '\n')
			{
				error_log("line %d: Missing comma\n", *line_index);
				result = FALSE;
				miss_comma = TRUE;
			}
		}
		else if (p[k] == ',')
		{
			k++;
			while (p[k] == ' ')
				k++;
		}
	}
	if (miss_comma)
	{
		for (s = 0; asm_line[s] == ' ' || asm_line[s] == '\t'; s++);
		for (s = s; asm_line[s] != ' ' && asm_line[s] != '\t'; s++);
		*(asm_line + s) = ',';
	}
	return result;
}


/**
 * @brief Extracts and determines the addressing modes of source and destination operands.
 *
 * This function parses an assembly instruction line to identify the source and destination operands,
 * determining their respective addressing modes. It first identifies the addressing mode of the source
 * operand, then looks for a comma to identify and determine the addressing mode of the destination operand.
 * If no comma is found, it assumes that the only operand is the destination, and the source operand is set to a space.
 *
 * @param asm_line A pointer to the assembly instruction line.
 * @param operand_src A pointer to a character where the addressing mode of the source operand will be stored.
 * @param operand_dst A pointer to a character where the addressing mode of the destination operand will be stored.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the operands' addressing modes were successfully identified.
 *         Returns TRUE if successful, FALSE if any errors occurred during parsing.
 */
boolean get_src_and_dst_operands(char* asm_line, char* operand_src, char* operand_dst, int* line_index) {
	boolean result = TRUE;
	int j;
	/*Discover the address method of source*/
	result &= get_addressing_mode(asm_line, operand_src, line_index);
	for (j = 0; asm_line[j] != '\0'; j++)
	{
		/*If thre is a comma, discover the address method of destination*/
		if (asm_line[j] == ',')
		{
			result &= get_addressing_mode(asm_line + j + 1, operand_dst, line_index);
			break;
		}
	}
	if (asm_line[j] == '\0')
	{
		/*If there isnt comma then the only operand is destination�*/
		*operand_dst = *operand_src;
		*operand_src = ' ';
	}
	return result;
}


/**
 * @brief Validates the opcode against the provided source and destination operands.
 *
 * This function checks whether the given opcode supports the addressing modes of the source and destination operands.
 * It compares the operand addressing modes against the allowed modes for the specific opcode.
 * If the addressing modes are not supported, it logs an error and returns FALSE.
 *
 * @param operand_src The addressing mode of the source operand.
 * @param operand_dst The addressing mode of the destination operand.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating whether the opcode is valid with the given operands.
 *         Returns TRUE if both the source and destination operands are valid for the opcode, FALSE otherwise.
 */
boolean validate_opcode_with_operands(char operand_src, char operand_dst, int* line_index) {
	int i = 0;
	boolean result = TRUE, is_src = FALSE, is_dst = FALSE;

	while ((operation_mode[opcode][1])[i] != '\0')
	{
		/*Checks if the specific op supports the source add method*/
		if ((operation_mode[opcode][1])[i] == operand_src)
			is_src = TRUE;
		i++;
	}
	i = 0;
	while ((operation_mode[opcode][0])[i] != '\0')
	{
		/*Checks whether the specific OP supports the destination add method*/
		if ((operation_mode[opcode][0])[i] == operand_dst)
			is_dst = TRUE;
		i++;
	}
	if (!is_src)
	{
		error_log("line %d: Incorrect method for the source operand\n", *line_index);
		result = FALSE;
	}
	if (!is_dst)
	{
		error_log("line %d: Incorrect method for the destination operand\n", *line_index);
		result = FALSE;
	}
	return result;
}


/**
 * @brief Configures a machine word with addressing modes and links it into the code table.
 *
 * This function sets up a `machine_word` structure by linking it into the existing code table, assigning
 * the role and address fields, and configuring the addressing modes for the source and destination operands.
 * The function handles specific combinations of direct and indirect register addressing modes and updates
 * the instruction counter (IC) accordingly.
 *
 * @param temp A pointer to the `machine_word` structure to be configured and linked.
 * @param operand_src The addressing mode of the source operand ('3' for direct register, '2' for indirect register).
 * @param operand_dst The addressing mode of the destination operand ('3' for direct register, '2' for indirect register).
 * @return A boolean value indicating success (TRUE) or failure (FALSE). Currently always returns TRUE.
 */
boolean allocate_and_configure_machine_word(machine_word* temp, char operand_src, char operand_dst) {
	temp->c.next = code_table[I].c.next;
	code_table[I].c.next = temp;
	code_table[I].c.role = 4; /*Its absolute*/
	code_table[I].c.address = IC; /*Give address*/
	IC++;
	code_table[I].c.next->c.address = IC;
	IC++;
	code_table[I].c.op_code = opcode;
	if (operand_src == '3' && operand_dst == '2')
	{
		code_table[I].c.source_direct_register = 1;
		code_table[I].c.destination_indirect_register = 1;
	}
	else if (operand_src == '2' && operand_dst == '3')
	{
		code_table[I].c.source_indirect_register = 1;
		code_table[I].c.destination_direct_register = 1;
	}
	else if (operand_src == '2' && operand_dst == '2')
	{
		code_table[I].c.source_indirect_register = 1;
		code_table[I].c.destination_indirect_register = 1;
	}
	else
	{
		code_table[I].c.source_direct_register = 1;
		code_table[I].c.destination_direct_register = 1;
	}
	return TRUE;
}


/**
 * @brief Configures the machine word for the destination operand in the code table.
 *
 * This function sets up a `machine_word` structure for the destination operand by linking it into the existing code table.
 * It assigns the role and address fields, sets the operation code, and configures the appropriate addressing mode
 * based on the provided destination operand. The function also ensures that the instruction counter (IC) is incremented
 * as addresses are assigned.
 *
 * @param temp A pointer to the `machine_word` structure that is being configured.
 * @param operand_dst The addressing mode of the destination operand. Valid values are:
 *        - '0': Immediate addressing mode
 *        - '1': Direct addressing mode
 *        - '2': Indirect register addressing mode
 *        - Any other value is treated as direct register addressing mode.
 * @return A boolean value indicating success (TRUE). Currently, the function always returns TRUE.
 */
boolean configure_destination_operand(machine_word* temp, char operand_dst) {
	temp->c.next = NULL;
	code_table[I].c.next = temp;
	code_table[I].c.role = 4; /*Its absolute*/
	code_table[I].c.address = IC; /*Give address*/
	IC++;
	code_table[I].c.next->c.address = IC;
	IC++;
	code_table[I].c.op_code = opcode;
	if (operand_dst == '0')
		code_table[I].c.destination_immidiate = 1;
	else if (operand_dst == '1')
		code_table[I].c.destination_direct = 1;
	else if (operand_dst == '2')
		code_table[I].c.destination_indirect_register = 1;
	else
		code_table[I].c.destination_direct_register = 1;
	return TRUE;
}


/**
 * @brief Handles the configuration of a machine word for instructions with no operands.
 *
 * This function sets up a `machine_word` structure in the code table for instructions that do not have any operands.
 * It assigns the role, address, and operation code fields, and ensures that no additional memory is allocated
 * by setting the `next` pointer to `NULL`. The instruction counter (IC) is incremented after the address is assigned.
 *
 * @return A boolean value indicating success (TRUE). The function always returns TRUE.
 */
boolean handle_no_operands() {
	code_table[I].c.role = 4; /*Its absolute*/
	code_table[I].c.address = IC; /*Give address*/
	IC++;
	code_table[I].c.op_code = opcode;
	code_table[I].c.next = NULL;
	return TRUE;
}


/**
 * @brief Configures a machine word for an instruction with both source and destination operands.
 *
 * This function sets up a `machine_word` structure for an instruction that involves both source and destination operands.
 * It handles the linking of the machine word into the existing code table, allocates additional memory for subsequent
 * machine words, sets the role and address fields, and configures the appropriate addressing modes for both the source
 * and destination operands. The instruction counter (IC) is incremented accordingly as addresses are assigned.
 *
 * @param temp A pointer to the `machine_word` structure that is being configured and linked into the code table.
 * @param operand_src The addressing mode of the source operand. Valid values are:
 *        - '0': Immediate addressing mode
 *        - '1': Direct addressing mode
 *        - '2': Indirect register addressing mode
 *        - Any other value is treated as direct register addressing mode.
 * @param operand_dst The addressing mode of the destination operand. Valid values are:
 *        - '0': Immediate addressing mode
 *        - '1': Direct addressing mode
 *        - '2': Indirect register addressing mode
 *        - Any other value is treated as direct register addressing mode.
 * @return A boolean value indicating success (TRUE) or failure (FALSE) due to memory allocation issues.
 */
boolean configure_dual_operand_instruction(machine_word* temp, char operand_src, char operand_dst) {
	temp->c.next = NULL;
	code_table[I].c.next = temp;
	temp = (machine_word*)malloc(sizeof(machine_word));
	if (temp == NULL)
	{
		error_log("Memory allocation failure");
		return FALSE;
	}
	temp->c.next = code_table[I].c.next;
	code_table[I].c.next = temp;
	code_table[I].c.role = 4; /*Its absolute*/
	code_table[I].c.address = IC; /*Give address*/
	IC++;
	code_table[I].c.next->c.address = IC;
	IC++;
	code_table[I].c.next->c.next->c.address = IC;
	IC++;
	code_table[I].c.op_code = opcode;
	/*For destination*/
	if (operand_dst == '0')
		code_table[I].c.destination_immidiate = 1;
	else if (operand_dst == '1')
		code_table[I].c.destination_direct = 1;
	else if (operand_dst == '2')
		code_table[I].c.destination_indirect_register = 1;
	else
		code_table[I].c.destination_direct_register = 1;
	/*For source*/
	if (operand_src == '0')
		code_table[I].c.source_immidiate = 1;
	else if (operand_src == '1')
		code_table[I].c.source_direct = 1;
	else if (operand_src == '2')
		code_table[I].c.source_indirect_register = 1;
	else
		code_table[I].c.source_direct_register = 1;
	return TRUE;
}


/**
 * @brief Processes an operation line in the assembly code.
 *
 * The operation function processes an operation line in the assembly code.
 * It handles operand addressing methods, checks for errors in operand syntax,
 * and allocates memory for machine words. The function handles different types
 * of operations and updates the code table accordingly.
 *
 * @param asm_line A pointer to the assembly line to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the operation line is processed successfully. Otherwise, returns FALSE.
 */
boolean operation(char* asm_line, int* line_index)
{
	boolean result = TRUE;
	char operand_src;
	char operand_dst = ' ';
	machine_word* temp;

	result &= handle_coma(asm_line, line_index);
	result &= get_src_and_dst_operands(asm_line, &operand_src, &operand_dst, line_index);
	result &= validate_opcode_with_operands(operand_src, operand_dst, line_index);

	temp = (machine_word*)malloc(sizeof(machine_word));
	if (temp == NULL)
	{
		error_log("Memory allocation failure");
		return FALSE;
	}

	if ((operand_src == '3' && operand_dst == '2') || (operand_dst == '3' && operand_src == '2') || (operand_src == '3' && operand_dst == '3') || (operand_dst == '2' && operand_src == '2'))
		result &= allocate_and_configure_machine_word(temp, operand_src, operand_dst);
	else if (operand_src == ' ' && operand_dst != ' ')
		result &= configure_destination_operand(temp, operand_dst);
	else if ((operand_src == ' ' && operand_dst == ' '))
		result &= handle_no_operands();
	else
		result &= configure_dual_operand_instruction(temp, operand_src, operand_dst);

	I++;
	return result;
}


/**
 * @brief Processes a line to check and handle an external label.
 *
 * The ext function processes a given line to determine if it contains a valid external
 * label. It removes leading spaces, checks for an empty line, validates the label's length,
 * and adds the label to the symbol table if valid. Errors are logged if the line is
 * empty, the label is too long, or memory allocation fails.
 *
 * @param asm_line A pointer to the line to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the line contains a valid external label and is processed
 *         successfully. Otherwise, returns FALSE.
 */
boolean ext(char* asm_line, int* line_index)
{
	boolean result = TRUE;
	symbol* temp;
	int i = 0;

	asm_line = delete_first_spaces(asm_line);
	if (*asm_line == '\0' || *asm_line == '\n') {
		error_log("line %d: Label is missing", *line_index);
		result = FALSE;
	}

	while (asm_line[i] != '\0' && asm_line[i] != '\n' && asm_line[i] != ' ') { i++; }
	if (i > MAX_LABEL_LENGTH)
	{
		error_log("line %d: Label is too long", *line_index);
		result = FALSE;
	}

	temp = (symbol*)malloc(sizeof(symbol));
	if (temp == NULL)
	{
		error_log("Memory allocation failure");
		return FALSE;
	}
	temp->next = head_symbol;
	head_symbol = temp;
	clean_label_name(head_symbol->symbol_name);
	strncpy(head_symbol->symbol_name, asm_line, i);
	head_symbol->is_external = TRUE;
	head_symbol->address = 0;
	return result;
}


/**
 * @brief Inserts numerical data from a line into the data table.
 *
 * The insert_numerical_data function processes a given line to extract numerical data.
 * It removes leading spaces, checks for missing or invalid parameters, and handles commas.
 * The function converts the extracted data from string format to integer format, and
 * inserts the data into the data table while updating the data counter.
 *
 * @param asm_line A pointer to the line containing numerical data to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the numerical data is successfully processed and inserted
 *         into the data table. Otherwise, returns FALSE.
 */
boolean insert_numerical_data(char* asm_line, int* line_index)
{
	boolean result = TRUE;
	int a[MAX_LINE_LENGTH];
	char b[MAX_LINE_LENGTH];
	int i = 0, j, z, counter;
	data_word* temp;
	asm_line = delete_first_spaces(asm_line);
	if (*asm_line == '\0' || *asm_line == '\n')
		error_log("line %d: Missing parameters", *line_index);
	while (*asm_line != '\0')
	{
		if (*asm_line == '-')
		{
			b[i] = '-';
			i++;
			asm_line++;
			if (!(*(asm_line) > 47 && *(asm_line) < 58))
			{
				error_log("line %d: Invalid parameter", *line_index);
				result = FALSE;
			}
		}
		else if (*asm_line == '+')
			asm_line++;
		asm_line = delete_first_spaces(asm_line);
		while ((*asm_line > 47 && *asm_line < 58))
		{
			b[i] = *asm_line;
			i++;
			asm_line++;
		}
		asm_line = delete_first_spaces(asm_line);
		if (*asm_line == ',')
		{
			b[i] = ',';
			i++;
			asm_line = delete_first_spaces(asm_line + 1);
			if (*(asm_line + 1) == ',')
			{
				error_log("line %d: Multiple number of consecutive commas\n", *line_index);
				result = FALSE;;
			}
		}
		else
		{
			asm_line = delete_first_spaces(asm_line);
			if ((*asm_line > 47 && *asm_line < 58))
			{
				error_log("line %d: Missing comma\n", *line_index);
				result = FALSE;
			}
			asm_line++;
		}
	}

	/* Convert the chars array to integers and fill the int array */
	j = 0;
	z = 0;
	counter = 1;
	while (j < i)
	{
		a[z] = 0;
		if (b[j] != ',')
		{
			a[z] = atoi(b + j);
			j++;
			z++;
		}
		if (b[j] == ',')
			counter++;
		j++;
	}
	DC += counter;
	z = 1;
	for (j = counter - 1; j >= 0; j--)
	{
		temp = (data_word*)malloc(sizeof(data_word));
		if (temp == NULL)
		{
			error_log("Memory allocation failure");
			return FALSE;
		}
		temp->d.next = data_table[D];
		data_table[D] = temp;
		data_table[D]->d.w = a[j];
		data_table[D]->d.address = DC - z;
		z++;
	}
	D++;
	return result;
}


/**
 * @brief Inserts string data from an assembly line into the data table.
 *
 * The insert_string_data function processes a given assembly line to extract string data.
 * It checks for proper syntax, validates the string length, and inserts the string data
 * into the data table. The function handles errors related to syntax, string length,
 * and memory allocation.
 *
 * @param asm_line A pointer to the assembly line containing string data to be processed.
 * @param line_index A pointer to the line counter.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the string data is successfully processed and inserted
 *         into the data table. Otherwise, returns FALSE.
 */
boolean insert_string_data(char* asm_line, int* line_index)
{
	boolean result = TRUE;
	int i = 0, j = 2, k;
	data_word* temp;

	asm_line = delete_first_spaces(asm_line);
	if (asm_line[0] != '"')
	{
		error_log("line %d: Syntax error missing", *line_index);
		result = FALSE;
	}

	while (asm_line[i] != '\0' && asm_line[i] != ' ') i++;
	if (i > MAX_STRING)
	{
		error_log("line %d: The string is too long, has more than 75 chars", *line_index);
		result = FALSE;
	}

	if (asm_line[i - 2] != '"')
	{
		error_log("line %d: Syntax error missing", *line_index);
		result = FALSE;
	}
	DC += i - 2;
	/*If its leggal string*/
	if (i > 3)
		i = i - 3;
	temp = (data_word*)malloc(sizeof(data_word));
	if (temp == NULL)
	{
		error_log("Memory allocation failure");
		return FALSE;
	}
	data_table[D] = temp;
	temp->d.next = NULL;
	data_table[D]->d.w = 0;
	data_table[D]->d.address = DC - 1;
	for (k = i; k >= 1; k--)
	{
		temp = (data_word*)malloc(sizeof(data_word));
		if (temp == NULL)
		{
			error_log("Memory allocation failure");
			return FALSE;
		}
		temp->d.next = data_table[D];
		data_table[D] = temp;
		data_table[D]->d.w = asm_line[k];
		data_table[D]->d.address = DC - j;
		j++;
	}
	D++;
	return result;
}