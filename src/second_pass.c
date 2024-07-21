#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/globals.h"
#include "../include/auxiliary.h"

extern int opcode; /*operation code*/

FILE* fd;
#define REGISTER_SYMBOL 'r'
#define DIRECT_SYMBOL '#'


void handle_error(const char* message) {
    printf("ERROR!! line %d: %s\n", line_counter, message);
    error_flag = ON;
}

/*
int find_symbol(const char* line, const char* cmp_str, int length) {
    return strncmp(line, cmp_str, length) ? YES : NO;
}*/


void update_machine_word(machine_word_fields_ptr machine_word, int w, int A) {
    machine_word->w = w;
    machine_word->A = A;
}


void handle_method_destination(char* li, code_word_fields_ptr code_word, machine_word_fields_ptr dest_machine_word) {
    symbol* temp;
    symbol* ext;
    int i;
    if (code_word->destination_direct_register)
        update_machine_word(dest_machine_word, atoi(li + 1), 1);
    else if (code_word->destination_indirect_register)
        update_machine_word(dest_machine_word, atoi(li + 2), 1);
    else if (code_word->destination_immidiate)
    {
        //li = find_next_symbol_in_line(li, DIRECT_SYMBOL);
        update_machine_word(dest_machine_word, atoi(li + 1), 1);
    }
    else if (code_word->destination_direct)
    {
        temp = head_symbol;
        for (i = 0; li[i] != '\0'; i++)
            if (li[i] == ' ' || li[i] == '\n')
                break;
        while (temp)
        {
            if (!strncmp(li, temp->symbol_name, i))
            {
                dest_machine_word->w = temp->address;
                if (temp->is_external)
                {
                    dest_machine_word->E = 1;
                    ext = (symbol*)malloc(sizeof(symbol));
                    if (!ext)
                    {
                        printf("Cannot allocate memory for ext\n");
                        return;
                    }
                    ext->next = head_externals;
                    head_externals = ext;
                    clean_label_name(head_externals->symbol_name);
                    strncpy(head_externals->symbol_name, temp->symbol_name, i);
                    head_externals->address = dest_machine_word->address;
                }
                else
                    dest_machine_word->R = 1;
                break;
            }
            temp = temp->next;
        }
    }
}


void handle_one_operand(char* li) {
    handle_method_destination(li, &code_table[I].c, &code_table[I].c.next->c);
    I++;
}


void handle_registers_method(char* asm_line, machine_word_fields_ptr machine_word) {
    /* First Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, atoi(asm_line + 1) << 3, 1);

    asm_line += 2; /* Move from the first register */

    /* Second Register */
    asm_line = find_next_symbol_in_line(asm_line, REGISTER_SYMBOL);
    update_machine_word(machine_word, machine_word->w + atoi(asm_line + 1), 1);
}

void handle_two_operands_method(char* li) {
    printf(li);
    symbol* temp;
    symbol* ext;
    int i;

    if (code_table[I].c.source_direct_register)
    {
        code_table[I].c.next->c.w = atoi(li + 1);
        code_table[I].c.next->c.w = code_table[I].c.next->c.w << 3;
        code_table[I].c.next->c.A = 1;
    }
    if (code_table[I].c.source_indirect_register)
    {
        code_table[I].c.next->c.w = atoi(li + 2);
        code_table[I].c.next->c.w = code_table[I].c.next->c.w << 3;
        code_table[I].c.next->c.A = 1;
    }
    if (code_table[I].c.source_immidiate)
    {
        //li = find_next_symbol_in_line(li, DIRECT_SYMBOL);
        code_table[I].c.next->c.w += atoi(li + 1);
        code_table[I].c.next->c.A = 1;
        li ++;
    }
    if (code_table[I].c.source_direct)
    {
        temp = head_symbol;
        for (i = 0; li[i] != '\0'; i++)
            if (li[i] == ',' || li[i] == ' ')
                break;
        while (temp)
        {
            if (!strncmp(li, temp->symbol_name, i))
            {
                code_table[I].c.next->c.w = temp->address;
                if (temp->is_external)
                {
                    code_table[I].c.next->c.E = 1;
                    ext = (symbol*)malloc(sizeof(symbol));
                    if (!ext)
                    {
                        printf("cannot allocate for ext\n");
                        return;
                    }
                    ext->next = head_externals;
                    head_externals = ext;
                    clean_label_name(head_externals->symbol_name);
                    strncpy(head_externals->symbol_name, temp->symbol_name, i);
                    head_externals->address = code_table[I].c.next->c.address;
                }
                else
                    code_table[I].c.next->c.R = 1;
                break;
            }
            temp = temp->next;
        }
    }
    li = find_comma(li);
    li += 1;
    li = delete_first_spaces(li);
    handle_method_destination(li, &code_table[I].c, &code_table[I].c.next->c.next->c);
}

boolean is_registry_method()
{
    return ((code_table[I].c.destination_indirect_register && code_table[I].c.source_direct_register) ||
            (code_table[I].c.destination_direct_register && code_table[I].c.source_indirect_register) ||
            (code_table[I].c.destination_indirect_register && code_table[I].c.source_indirect_register) ||
            (code_table[I].c.destination_direct_register && code_table[I].c.source_direct_register));
}

void handle_two_operands(char* li)
{
    if (is_registry_method())
        handle_registers_method(li, &code_table[I].c.next->c);
    else
        handle_two_operands_method(li);
    I++;
}


/*This function passes a second time on the operations in the text, Classifies and allocates memory - machine words*/
void second_operation(char* li)
{
	li = delete_first_spaces(li);
	/*Operations without operands*/
	if (opcode == 14 || opcode == 15)
		I++;
	/*Operations with one operand only*/
	else if (opcode <= 13 && opcode >= 5)
        handle_one_operand(li);
	/*Operations with two operands*/
	else if (opcode <= 4 && opcode >= 0)
	{
        handle_two_operands(li);
	}
}


int isLabel2(char* line)
{
    int i;
    line = delete_first_spaces(line);
    for (i = 0; i < MAX_LINE_LENGTH; i++)
    {
        if (line[i] == ':')
            return 1;
    }
    return 0;
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


void analize_2_second_pass(char* l)
{
    /*l1 is a point in the first place right after the first spaces*/
    char* l1 = delete_first_spaces(l);
    /*This condition continues the text transition as long as the line is blank or consist comments*/
    if (!strncmp(l1, ".entry", strlen(".entry")))
    {
        process_entry(l1 + 6);
        return;
    }
    if (isLabel2(l1))
    {
        process_lable(l1);
        return;
    }
    if (is_operation(l1))
    {
        second_operation(l1 + OPERATION_LENGTH);
        return;
    }
    if (is_stop(l1))
    {
        second_operation(l1 + STOP_LENGTH);
        return;
    }
}


int second_pass_exec(FILE* file_handle)
{
    fd = file_handle;
    /*Second pass*/
    while (!feof(fd))
    {
        /*Second analize*/
        analize_2_second_pass(line);
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, fd);
    }

    return validate_second_pass();
}