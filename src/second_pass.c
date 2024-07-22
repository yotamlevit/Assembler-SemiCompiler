#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/globals.h"
#include "../include/logger.h"

extern int opcode; /*operation code*/

FILE* fd;
#define REGISTER_SYMBOL 'r'
#define COMMA ','
#define SPACE ' '
#define NEW_LINE '\n'
#define LABLE_SYMBOL ':'


/*
int find_symbol(const char* line, const char* cmp_str, int length) {
    return strncmp(line, cmp_str, length) ? YES : NO;
}*/


void update_machine_word(machine_word_fields_ptr machine_word, int w, int A) {
    machine_word->w = w;
    machine_word->A = A;
}


int get_operand_offset_value(code_word_fields_ptr code_word, boolean is_src)
{
    if (((code_word->destination_direct_register || code_word->destination_immidiate) && is_src == NO) || (( code_word->source_immidiate || code_word->source_direct_register) && is_src == YES))
        return 1;
    else if ((code_word->destination_indirect_register && is_src == NO) || (code_word->source_indirect_register && is_src == YES))
        return 2;
    else
        return 0;
}

boolean handle_operand(char* asm_line, code_word_fields_ptr code_word, machine_word_fields_ptr dest_machine_word, boolean is_src) {
    symbol* temp;
    symbol* ext;
    int symbol_name_length, offset_value;


    if ((code_word->destination_direct && is_src == NO) || (code_word->source_direct && is_src == YES))
    {
        temp = head_symbol;

        for (symbol_name_length = 0; asm_line[symbol_name_length] != COMMA && asm_line[symbol_name_length] != SPACE && asm_line[symbol_name_length] != NEW_LINE && asm_line[symbol_name_length] != END_OF_STR; symbol_name_length++);

        while (temp)
        {
            if (!strncmp(asm_line, temp->symbol_name, symbol_name_length))
            {
                dest_machine_word->w = temp->address;
                if (temp->is_external)
                {
                    dest_machine_word->E = 1;
                    ext = (symbol*)malloc(sizeof(symbol));
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
                break;
            }
            temp = temp->next;
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

        if (is_src)
            update_machine_word(dest_machine_word, atoi(asm_line + offset_value) << 3, 1);
        else
            update_machine_word(dest_machine_word, atoi(asm_line + offset_value), 1);
    }

    return YES;
}


boolean handle_one_operand(char* li) {
    /* Handling dest operand */
    return handle_operand(li, &code_table[I].c, &code_table[I].c.next->c, NO);
}


boolean handle_registers_method(char* asm_line, machine_word_fields_ptr machine_word) {
    /* First Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, atoi(asm_line + 1) << 3, 1);

    asm_line += 2; /* Move from the first register */

    /* Second Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, machine_word->w + atoi(asm_line + 1), 1);

    return YES;
}

boolean handle_two_operands_method(char* li) {
    boolean result = YES;
    /* Handle Source operand */
    result = handle_operand(li, &code_table[I].c, &code_table[I].c.next->c, YES);

    if(result != NO)
    {
        /* Move from the first operand */
        li = find_comma(li);
        li += 1;
        li = delete_first_spaces(li);

        /* Handle Destination operand */
        result = handle_operand(li, &code_table[I].c, &code_table[I].c.next->c.next->c, NO);
    }

    return result;


}

boolean is_registry_method()
{
    return ((code_table[I].c.destination_indirect_register && code_table[I].c.source_direct_register) ||
            (code_table[I].c.destination_direct_register && code_table[I].c.source_indirect_register) ||
            (code_table[I].c.destination_indirect_register && code_table[I].c.source_indirect_register) ||
            (code_table[I].c.destination_direct_register && code_table[I].c.source_direct_register));
}

boolean handle_two_operands(char* li)
{
    if (is_registry_method())
        return handle_registers_method(li, &code_table[I].c.next->c);
    else
        return handle_two_operands_method(li);
}


/*This function passes a second time on the operations in the text, Classifies and allocates memory - machine words*/
boolean second_operation(char* li)
{
    boolean result = YES;
	li = delete_first_spaces(li);
	/*Operations with one operand only*/
	if (opcode <= 13 && opcode >= 5)
        result = handle_one_operand(li);
	/*Operations with two operands*/
	else if (opcode <= 4 && opcode >= 0)
        result = handle_two_operands(li);

    I++;
    return result;
}


int isLabel2(char* asm_line)
{
    return *find_next_symbol_in_line(asm_line, LABLE_SYMBOL) == LABLE_SYMBOL;
}


/*This function is called if the word is a label*/
void process_lable(char* line)
{
    int i;
    for (i = 0; i < MAX_LINE_LENGTH; i++)
    {
        if (line[i] == ':')
        {
            /*go again to analize2 */
            analize_2_second_pass(line + i + 1);
            return;
        }
    }
}


/*This function creates a list that handles the entry guides*/
void process_entry(char* li)
{
    symbol* temp = head_symbol;
    symbol* temp2;
    boolean is_exist = NO;
    int i;
    li = delete_first_spaces(li);
    if (*li == '\0')
    {
        printf("ERROR!! line %d: Label is Missing\n", line_counter);
        error_flag = ON;
    }
    if (strlen(li) > MAX_LABEL_LENGTH)
    {
        printf("ERROR!! line %d: Label is too long\n", line_counter);
        error_flag = ON;
    }
    for (i = 1; i < MAX_LINE_LENGTH || li[i] == '\0'; i++)
    {
        if (li[i] == ' ' || li[i] == '\n' || li[i] == '\t')
            break;
    }
    while (temp)
    {
        if (!strncmp(li, temp->symbol_name, i))
        {
            is_exist = YES;
            if (temp->is_external)
            {
                printf("ERROR!! line %d: This label is external\n", line_counter);
                error_flag = ON;
            }
            else
            {
                /*Create a list of entry labels*/
                temp2 = (symbol*)malloc(sizeof(symbol));
                if (!temp2)
                {
                    printf("No memmory\n");
                    break;
                }
                temp2->next = head_entries;
                head_entries = temp2;
                clean_label_name(head_entries->symbol_name);
                head_entries->address = temp->address;
                strcpy(head_entries->symbol_name, temp->symbol_name);
            }
        }
        temp = temp->next;
    }
    if (is_exist == NO)
    {
        printf("ERROR!! line %d: Label is not exist\n", line_counter);
        error_flag = ON;
    }
}


boolean analize_2_second_pass(char* asm_line)
{
    /* Clean the line from spaces */
    asm_line = delete_first_spaces(asm_line);

    if (!strncmp(asm_line, ".entry", strlen(".entry")))
    {
        process_entry(asm_line + 6);
        return YES;
    }
    if (isLabel2(asm_line))
    {
        process_lable(asm_line);
        return YES;
    }
    /* Otherwise it is an operation*/
    if (is_operation(asm_line))
        return second_operation(asm_line + OPERATION_LENGTH);
    if (is_stop(asm_line))
        return second_operation(asm_line + STOP_LENGTH);

    /* If here then it is a .string, .data or .extern */
    return YES;
}


int second_pass_exec(FILE* file_handle)
{
    fd = file_handle;
    /*Second pass*/
    while (!feof(fd))
    {
        /*Second analize*/
        if (!analize_2_second_pass(line))
        {
            error_log("Second pass failed on line: %s", delete_first_spaces(line));
            return NO;
        }
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, fd);
    }

    return validate_second_pass();
}