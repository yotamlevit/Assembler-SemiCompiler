#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/globals.h"
#include "../include/auxiliary.h"

extern int opcode; /*operation code*/

FILE* fd;

/*End of analize_2_second_pass*/

/*End of second_operation*/



void handle_error(const char* message) {
    printf("ERROR!! line %d: %s\n", line_counter, message);
    error_flag = ON;
}

/*
int find_symbol(const char* line, const char* cmp_str, int length) {
    return strncmp(line, cmp_str, length) ? YES : NO;
}*/



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

/*End of ent*/


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


void handle_one_operand(char* li) {
    symbol* temp;
    symbol* ext;
    int i;
    /*For method 0*/
    if (code_table[I].c.destination_immidiate)
    {
        /*Converts the instant number*/
        code_table[I].c.next->c.w = atoi(li + 1);
        code_table[I].c.next->c.A = 1;
    }
    else if (code_table[I].c.destination_direct)
    {
        temp = head_symbol;
        for (i = 0; li[i] != '\0'; i++)
            if (li[i] == ' ' || li[i] == '\n')
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
                        printf("Can not allocate memory for ext.\n");
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
    else if (code_table[I].c.destination_direct_register)
    {
        code_table[I].c.next->c.w = atoi(li + 1);
        code_table[I].c.next->c.A = 1;
    }
    else if (code_table[I].c.destination_indirect_register)
    {
        code_table[I].c.next->c.w = atoi(li + 2);
        code_table[I].c.next->c.A = 1;
    }
    I++;
}


void handle_two_operands(char* li)
{
    symbol* temp;
    symbol* ext;
    int i;

    if ((code_table[I].c.destination_indirect_register && code_table[I].c.source_direct_register) || (code_table[I].c.destination_direct_register && code_table[I].c.source_indirect_register) || (code_table[I].c.destination_indirect_register && code_table[I].c.source_indirect_register) || (code_table[I].c.destination_direct_register && code_table[I].c.source_direct_register))
    {
        for (i = 0; li[i] != '\0'; i++)
        {
            if (li[i] == 'r')
            {
                code_table[I].c.next->c.w = atoi(li + i + 1);
                code_table[I].c.next->c.w = code_table[I].c.next->c.w << 3;
                code_table[I].c.next->c.A = 1;
                li += i + 1;
                break;
            }
        }
        for (i = 0; li[i] != '\0'; i++)
        {
            if (li[i] == 'r')
            {
                code_table[I].c.next->c.w += atoi(li + i + 1);
                code_table[I].c.next->c.A = 1;
                li += i;
                break;
            }
        }
    }
        /*Operations with two operands and two machine words*/
    else
    {
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
            for (i = 0; li[i] != '\0'; i++)
            {
                if (li[i] == '#')
                {
                    code_table[I].c.next->c.w += atoi(li + i + 1);
                    code_table[I].c.next->c.A = 1;
                    li += i + 1;
                    break;
                }
            }
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
        if (code_table[I].c.destination_direct_register)
        {
            code_table[I].c.next->c.next->c.w = atoi(li + 1);
            code_table[I].c.next->c.next->c.A = 1;
        }
        if (code_table[I].c.destination_indirect_register)
        {
            code_table[I].c.next->c.next->c.w = atoi(li + 2);
            code_table[I].c.next->c.next->c.A = 1;
        }
        if (code_table[I].c.destination_immidiate)
        {
            for (i = 0; li[i] != '\0'; i++)
            {
                if (li[i] == '#')
                {
                    code_table[I].c.next->c.next->c.w += atoi(li + i + 1);
                    code_table[I].c.next->c.next->c.A = 1;
                    break;
                }
            }
        }
        if (code_table[I].c.destination_direct)
        {
            temp = head_symbol;
            for (i = 0; li[i] != '\0'; i++)
                if (li[i] == ' ' || li[i] == '\n')
                    break;
            while (temp)
            {
                if (!strncmp(li, temp->symbol_name, i))
                {
                    code_table[I].c.next->c.next->c.w = temp->address;
                    if (temp->is_external)
                    {
                        code_table[I].c.next->c.next->c.E = 1;
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
                        head_externals->address = code_table[I].c.next->c.next->c.address;
                    }
                    else
                        code_table[I].c.next->c.next->c.R = 1;
                    break;
                }
                temp = temp->next;
            }
        }
    }
    I++;
    /*End of else*/
}


/*This function passes a second time on the operations in the text, Classifies and allocates memory - machine words*/
void second_operation(char* li)
{
	symbol* temp = head_symbol;
	symbol* ext;
	int i;
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
	/*End of operations with two operands*/
}