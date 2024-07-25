#include "../include/utils.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/logger.h"


/**
 * Calculates the length of a symbol name in an assembly line.
 *
 * @param asm_line The assembly line containing the symbol.
 * @return The length of the symbol name.
 */
int get_symbol_length(const char* asm_line) {
    int symbol_name_length;
    for (symbol_name_length = 0; asm_line[symbol_name_length] != COMMA && asm_line[symbol_name_length] != SPACE && asm_line[symbol_name_length] != NEW_LINE && asm_line[symbol_name_length] != END_OF_STR; symbol_name_length++);

    return symbol_name_length;
}


/**
 * Finds a symbol in the symbol table by its name.
 *
 * @param asm_line The assembly line containing the symbol.
 * @param symbol_name_length The length of the symbol name.
 * @return A pointer to the symbol if found, otherwise NULL.
 */
symbol_ptr find_symbol(const char* asm_line, int symbol_name_length) {
    symbol_ptr temp = head_symbol;
    while (temp) {
        if (!strncmp(asm_line, temp->symbol_name, symbol_name_length)) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}


/**
 * Updates a machine word with specified values.
 *
 * @param machine_word A pointer to the machine word to be updated.
 * @param w The value to be set for the word.
 * @param A The value to be set for the A field.
 * @param is_src A boolean indicating if the word is a source operand.
 */
void update_machine_word(machine_word_fields_ptr machine_word, int w, int A, boolean is_src) {
    machine_word->w = is_src ? w << 3 : w;
    machine_word->A = A;
}


/**
 * Calculates the offset value for an operand.
 *
 * @param code_word A pointer to the code word containing the operand.
 * @param is_src A boolean indicating if the operand is a source operand.
 * @return The offset value for the operand.
 */
int get_operand_offset_value(code_word_fields_ptr code_word, boolean is_src)
{
    if (((code_word->destination_direct_register || code_word->destination_immidiate) && is_src == NO) ||
        (( code_word->source_immidiate || code_word->source_direct_register) && is_src == YES))
        return 1;

    if ((code_word->destination_indirect_register && is_src == NO) ||
        (code_word->source_indirect_register && is_src == YES))
        return 2;

    return 0;
}


/**
 * Handles an operand in an assembly line and updates the machine word.
 *
 * @param asm_line The assembly line containing the operand.
 * @param code_word A pointer to the code word containing the operand.
 * @param dest_machine_word A pointer to the destination machine word.
 * @param is_src A boolean indicating if the operand is a source operand.
 * @return A boolean indicating success or failure.
 */
boolean handle_operand(char* asm_line, code_word_fields_ptr code_word, machine_word_fields_ptr dest_machine_word, boolean is_src) {
    symbol_ptr temp, ext;
    int symbol_name_length, offset_value;

    if ((code_word->destination_direct && is_src == NO) || (code_word->source_direct && is_src == YES))
    {
        symbol_name_length = get_symbol_length(asm_line);

        temp = find_symbol(asm_line, symbol_name_length);

        if (temp){
            dest_machine_word->w = temp->address;
            if (temp->is_external)
            {
                dest_machine_word->E = 1;
                ext = (symbol_ptr)malloc(sizeof(symbol));
                if (!ext)
                {
                    error_log("Cannot allocate memory for ext (second pass) - raised on line: %s", asm_line);
                    return NO;
                }
                ext->next = head_externals;
                head_externals = ext;
                clean_label_name(head_externals->symbol_name);
                strncpy(head_externals->symbol_name, temp->symbol_name, symbol_name_length);
                head_externals->address = dest_machine_word->address;
            }
            else
                dest_machine_word->R = 1;
        }
    }
    else
    {
        /* is not a direct_memory */
        offset_value = get_operand_offset_value(code_word, is_src);

        if(offset_value == 0)
        {
            error_log("Error handling operand %s - raised on line: %s", is_src ? "source" : "destination", asm_line);
            return NO;
        }

        update_machine_word(dest_machine_word, atoi(asm_line + offset_value), 1, is_src);
    }

    return YES;
}


/**
 * Handles an assembly line with one operand.
 *
 * @param asm_line The assembly line containing the operand.
 * @param code_word A pointer to the code word containing the operand.
 * @return A boolean indicating success or failure.
 */
boolean handle_one_operand(char* asm_line, code_word_fields_ptr code_word) {
    /* Handling dest operand */
    return handle_operand(asm_line, code_word, &code_word->next->c, NO);
}


/**
 * Handles an assembly line with register operands.
 *
 * @param asm_line The assembly line containing the register operands.
 * @param machine_word A pointer to the machine word to be updated.
 * @return A boolean indicating success or failure.
 */
boolean handle_registers_method(char* asm_line, machine_word_fields_ptr machine_word) {
    /* First Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, atoi(asm_line + 1), 1, YES);

    asm_line += 2; /* Move from the first register */

    /* Second Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, machine_word->w + atoi(asm_line + 1), 1, NO);

    return YES;
}


/**
 * Handles an assembly line with two operands.
 *
 * @param asm_line The assembly line containing the operands.
 * @param code_word A pointer to the code word containing the operands.
 * @return A boolean indicating success or failure.
 */
boolean handle_two_operands_method(char* asm_line, code_word_fields_ptr code_word) {
    boolean result;
    /* Handle Source operand */
    result = handle_operand(asm_line, code_word, &code_word->next->c, YES);

    if(result != NO)
    {
        /* Move from the first operand */
        asm_line = find_next_symbol_in_line(asm_line, COMMA);
        asm_line++;
        asm_line = delete_first_spaces(asm_line);

        /* Handle Destination operand */
        result = handle_operand(asm_line, code_word, &code_word->next->c.next->c, NO);
    }

    return result;


}


/**
 * Checks if the method is a registry method.
 *
 * @return A boolean indicating if the method is a registry method.
 */
boolean is_registry_method(code_word_fields_ptr code_word)
{
    return ((code_word->destination_indirect_register && code_word->source_direct_register) ||
            (code_word->destination_direct_register && code_word->source_indirect_register) ||
            (code_word->destination_indirect_register && code_word->source_indirect_register) ||
            (code_word->destination_direct_register && code_word->source_direct_register));
}


/**
 * Handles an assembly line with two operands, dispatching to the appropriate method.
 *
 * @param asm_line The assembly line containing the operands.
 * @param code_word A pointer to the code word containing the operands.
 * @return A boolean indicating success or failure.
 */
boolean handle_two_operands(char* asm_line, code_word_fields_ptr code_word)
{
    if (is_registry_method(code_word))
        return handle_registers_method(asm_line, &code_word->next->c);
    else
        return handle_two_operands_method(asm_line, code_word);
}


/**
 * Performs the second pass operation on an assembly line.
 *
 * @param asm_line The assembly line to be processed.
 * @param code_word A pointer to the code word for the line.
 * @return A boolean indicating success or failure.
 */
boolean second_operation(char* asm_line, code_word_fields_ptr code_word, int* line_index, int* opcode)
{
    boolean result = YES;
    asm_line = delete_first_spaces(asm_line);
	/*Operations with one operand only*/
	if (*opcode <= 13 && *opcode >= 5)
        result = handle_one_operand(asm_line, code_word);
	/*Operations with two operands*/
	else if (*opcode <= 4 && *opcode >= 0)
        result = handle_two_operands(asm_line, code_word);

    *(line_index) += 1;
    return result;
}


/**
 * Checks if an assembly line contains a label.
 *
 * @param asm_line The assembly line to be checked.
 * @return A boolean indicating if the line contains a label.
 */
boolean isLabel2(char* asm_line)
{
    return *find_next_symbol_in_line(asm_line, LABEL_SYMBOL) == LABEL_SYMBOL;
}


/**
 * Processes a label in an assembly line.
 *
 * @param asm_line The assembly line containing the label.
 * @param code_word A pointer to the code word for the line.
 * @return A boolean indicating success or failure.
 */
boolean process_label(char* asm_line, code_word_fields_ptr code_word, int* line_index)
{
    asm_line = find_next_symbol_in_line(asm_line, LABEL_SYMBOL);
    return process_line(++asm_line, code_word, line_index);
}


/**
 * Processes an entry directive in an assembly line.
 *
 * @param asm_line The assembly line containing the entry directive.
 * @return A boolean indicating success or failure.
 */
boolean process_entry(char* asm_line)
{
    symbol_ptr temp, temp2;
    boolean is_lable_exist = NO;
    int symbol_name_length;

    asm_line = delete_first_spaces(asm_line);

    if (*asm_line == END_OF_STR)
    {
        error_log("Label is missing on line %s", asm_line);
        return NO;
    }

    symbol_name_length = get_symbol_length(asm_line);

    temp = find_symbol(asm_line, symbol_name_length);

    if (temp){
        is_lable_exist = YES;
        if (!temp->is_external)
        {
            /*Create a list of entry labels*/
            temp2 = (symbol_ptr)malloc(sizeof(symbol));
            if (!temp2)
            {
                error_log("Cannot allocate memory for entry labels (second pass) - raised on line: %s", asm_line);
                return NO;
            }
            temp2->next = head_entries;
            head_entries = temp2;
            clean_label_name(head_entries->symbol_name);
            head_entries->address = temp->address;
            strcpy(head_entries->symbol_name, temp->symbol_name);
        }
    }

    if (!is_lable_exist)
    {
        error_log("Label does not exist on line %s", asm_line);
        return NO;
    }

    return YES;
}


/**
 * Processes a single line of assembly code.
 *
 * @param asm_line The assembly line to be processed.
 * @param code_word A pointer to the code word for the line.
 * @return A boolean indicating success or failure.
 */
boolean process_line(char* asm_line, code_word_fields_ptr code_word, int* line_index)
{
    int opcode;
    /* Clean the line from spaces */
    char* clean_line = delete_first_spaces(asm_line);
    if (!strncmp(clean_line, ENTRY_LABEL, strlen(ENTRY_LABEL)))
        return process_entry(clean_line + 6);
    if (isLabel2(clean_line))
        return process_label(clean_line, code_word, line_index);
    /* Otherwise it is an operation*/
    opcode = is_operation(clean_line);
    if (opcode != -1)
        return second_operation(clean_line + OPERATION_LENGTH, code_word, line_index, &opcode);
    opcode = is_stop(clean_line);
    if (opcode != -1)
        return second_operation(clean_line + STOP_LENGTH, code_word, line_index, &opcode);

    /* If here then it is a .string, .data or .extern */
    return YES;
}


/**
 * Executes the second pass on the assembly file.
 *
 * @param file_handle The file handle for the assembly file.
 * @return A boolean indicating success or failure.
 */
boolean second_pass_exec(FILE* file_handle, int* line_index)  //// TODO WHen changing the global get an argumentof code_table //// TODO Change boolean to status?
{
    /*Second pass*/
    while (!feof(file_handle))
    {
        /*Second analize*/
        if (!process_line(line, &code_table[*line_index].c, line_index))
        {
            error_log("Second pass failed on line: %s", delete_first_spaces(line));
            return NO;
        }
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, file_handle);
    }

    return YES;
}