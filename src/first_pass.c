#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/tables.h"
#include "../include/first_pass.h"
#include "../include/constants_tables.h"
#include "../include/status_codes.h"
#include "globals.h"

/*Operation code.Reliable only when the action is valid*/
int opcode; 

/*The first pass of the main function*/
void analize_input_line(char* l)
{
	/*l1 is a point in the first place right after the first spaces*/
	char* l1 = delete_first_spaces(l); 
	/*This condition continues the text transition as long as the line is blank or consist comments*/
	if (*l1 == ';' || *l1 == '\0' || *l1 == '\n') 
		return;
	if (!strncmp(l1, ".entry", 6))
		return;
	if (!strncmp(l1, ".extern", 7))
	{
		ext(l1 + 7);
		return;
	}
	if (!strncmp(l1, ".data", 5))
	{
		insert_numerical_data(l1 + 5);
		return;
	}
	if (!strncmp(l1, ".string", 7))
	{
		insert_string_data(l1 + 7);
		return;
	}
	opcode = is_operation(l1);
    if (opcode != -1) // In second pass refactor i remove opcode global from the function
	{
		operation(l1 + 3);
		return;
	}
	opcode = is_stop(l1);
	if (opcode != -1)/*if it is stop operation*/ // In second pass refactor i remove opcode global from the function
	{
		operation(l1 + 4);
		return;
	}
	if (is_label(l1))
	{
		label_actions(l1);
		return;
	}
	printf("ERROR!! line %d: The command was not found\n", line_counter);
	error_flag = ON;
}
/*End of analize_input_line functiion*/

/*End of is_label function*/

/*End of label_actions function*/

/*End of is_operation function*/

/*End of is_stop function*/

/*End of operation function*/

 /*Address method*/
char addressing_mode(char* li)
{ 
	int i = 2;
	li = delete_first_spaces(li);
	if (*li == '#')
	{
		/*If after # does not appear a number - throw an error*/
		if (li[1] < 47 || li[1]>58)
		{ 
			if (li[1] != '-' && li[1] != '+')
			{
				printf("ERROR!! line %d: Invalid parameter for the instant address\n", line_counter);
				error_flag = ON;
			}
		}
		else
		{
			while (li[i] != ' ' && li[i] != ',' && li[i] != '\0' && li[i] != '\n' && li[i] != '\t')
			{
				/*If after # does not appear a number - throw an error*/
				if (li[i] < 47 || li[i]>58)
				{  
					printf("ERROR!! line %d: Invalid parameter for the instant address\n", line_counter);
					error_flag = ON;
					break;
				}
				i++;
			}
		}
		/*Immediate address*/
		return '0'; 
	}
	/*Indirect register address*/
	else if (*li == '*')
	{ 
		if (!(*(li + 1) == 'r'))
		{
			printf("ERROR!! line %d: Invalid override parameter\n", line_counter);
			error_flag = ON;
		}
		/*If the register is not between 0-7*/
		else if (!(*(li + 2) > 47 && *(li + 2) < 56)) 
		{
			printf("ERROR!! line %d: Invalid indirect address registration\n", line_counter);
			error_flag = ON;
		}
		return '2';
	}
	/*Direct register address*/
	else if (*li == 'r')
	{ 
		/*If the register is not between 0-7*/
		if (!(*(li + 1) > 47 && *(li + 1) < 56)) 
		{
			printf("ERROR!! line %d: Invalid direct address registration\n", line_counter);
			error_flag = ON;
		}
		return '3';
	}
	/*The operand is a label - Direct address*/
	else if (*li > 20 && *li < 127) 
		return '1';
	else
		return ' ';
}/*End of addressing_mode function*/

/*End of ext function*/

/*End of insert_numerical_data function*/

/*End of insert_string_data function��*/

/*End of fix_symbol_addresses function*/

/*This function updates the symbols address in the symbols list*/
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

int first_pass_exec(FILE* file_handle)
{
    line_counter = 0;
    error_flag=OFF;
    /*First pass*/
    while (!feof(file_handle))
    {
        /*First analize*/
        analize_input_line(line);
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, file_handle);
    }

    //exe_first_pass(file_name);
    validate_memory(IC, DC);
	return success;
}

/*This function checks if there is a label in the begining*/
int is_label(char* li)
{
	int i;
	char* p;
	li = delete_first_spaces(li);
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		if (li[i] == ':')
			return 1;
		else if (li[i] == ' ')
		{
			p = li;
			p = delete_first_spaces(p);
			if (li[i] != '\0')
			{
				printf("ERROR!! line %d: Missing : in order to be label\n", line_counter);
				error_flag = ON;
				*(p - 1) = ':';
				return 1;
			}
		}
	}
	return 0;
}

/*This function receives a word - if it is a label, and takes action on it*/
void label_actions(char* li)
{
	char* p;
	symbol* temp;
	int i;
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		if (li[i] == ':')
		{
			if (i > MAX_LABEL_LENGTH)
			{
				printf("ERROR!! line %d: Lable is too long, has more than 30 chars\n", line_counter);
				error_flag = ON;
			}
			else
			{
				temp = (symbol*)malloc(sizeof(symbol));
				if (!temp)
				{
					printf("no memmory\n");
					break;
				}
				temp->next = head_symbol;
				head_symbol = temp;
				clean_label_name(head_symbol->symbol_name);
				strncpy(head_symbol->symbol_name, li, i);
			}
			p = (li + i + 1);
			p = delete_first_spaces(p);
			if (*p == '.')
			{
				/*If its guide statement*/
				head_symbol->address = DC;
				head_symbol->is_attached_directive = YES;
				head_symbol->is_external = NO;
				if (!strncmp(p, ".entry", 6))
					printf("WARNING!! line %d: A label defined at the beginig of entry statement is ignored\n" ,line_counter);
				else if (!strncmp(p, ".extern", 7))
					printf("WARNING!! line %d: A label defined at the beginig of extern statement is ignored.\n", line_counter);
				else
					/*Sends again to analize to find out if its string or data*/
					analize_input_line(p);
			}
			else
			{
				/*If this is a statement of instruction*/
				head_symbol->address = IC;
				head_symbol->is_attached_directive = NO;
				head_symbol->is_external = NO;
				/*Go again to analize to find out which instruction statement*/
				analize_input_line(p);
			}
		}
	}
}

/*The first transition of the operations*/
void operation(char* li)
{
	int k;
	int s;
	int j;
	char oper[MAX_LINE_LENGTH - 4];
	char* p = oper;
	/*Method of addrresing operand source*/
	char operand_source;
	/*Method of addrresing operand destanation*/
	char operand_destination = ' ';
	int i = 0;
	boolean is_source = NO;
	boolean is_destination = NO;
	machine_word* temp;
	boolean miss_comma = 0;
	for (k = 0; li[k] == ' ' || li[k] == '\t'; k++);
	strcpy(oper, li + k);
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
				printf("ERROR!! line %d: Missing comma\n", line_counter);
				error_flag = ON;
				miss_comma = YES;
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
		for (s = 0; li[s] == ' ' || li[s] == '\t'; s++);
		for (s = s; li[s] != ' ' && li[s] != '\t'; s++);
		*(li + s) = ',';
	}
	/*Discover the address method of source*/
	operand_source = addressing_mode(li);
	for (j = 0; li[j] != '\0'; j++)
	{
		/*If thre is a comma, discover the address method of destination*/
		if (li[j] == ',')
		{
			operand_destination = addressing_mode(li + j + 1);
			break;
		}
	}
	if (li[j] == '\0')
	{
		/*If there isnt comma then the only operand is destination�*/
		operand_destination = operand_source;
		operand_source = ' ';
	}
	while ((operation_mode[opcode][1])[i] != '\0')
	{
		/*Checks if the specific op supports the source add method*/
		if ((operation_mode[opcode][1])[i] == operand_source)
			is_source = YES;
		i++;
	}
	i = 0;
	while ((operation_mode[opcode][0])[i] != '\0')
	{
		/*Checks whether the specific OP supports the destination add method*/
		if ((operation_mode[opcode][0])[i] == operand_destination)
			is_destination = YES;
		i++;
	}
	/*If not rise an error*/
	if (!is_source)
	{
		printf("ERROR!! line %d: Incorrect method for the source operand\n", line_counter);
		error_flag = ON;
	}
	if (!is_destination)
	{
		printf("ERROR!! line %d: Incorrect method for the destination operand\n", line_counter);
		error_flag = ON;
	}
	/*If there is 2 operands and both of them 3 or 4 add method, they share the same memmory word--allocate one more word */
	if ((operand_source == '3' && operand_destination == '2') || (operand_destination == '3' && operand_source == '2') || (operand_source == '3' && operand_destination == '3') || (operand_destination == '2' && operand_source == '2'))
	{
		temp = (machine_word*)malloc(sizeof(machine_word));
		if (!temp)
		{
			printf("ERORR!! Memory allocation faild\n");
			error_flag = ON;
			return;
		}
		temp->c.next = code_table[I].c.next;
		code_table[I].c.next = temp;
		code_table[I].c.role = 4; /*Its absolute*/
		code_table[I].c.address = IC; /*Give address*/
		IC++;
		code_table[I].c.next->c.address = IC;
		IC++;
		code_table[I].c.op_code = opcode;
		if (operand_source == '3' && operand_destination == '2')
		{
			code_table[I].c.source_direct_register = 1;
			code_table[I].c.destination_indirect_register = 1;
		}
		else if (operand_source == '2' && operand_destination == '3')
		{
			code_table[I].c.source_indirect_register = 1;
			code_table[I].c.destination_direct_register = 1;
		}
		else if (operand_source == '2' && operand_destination == '2')
		{
			code_table[I].c.source_indirect_register = 1;
			code_table[I].c.destination_indirect_register = 1;
		}
		else
		{
			code_table[I].c.source_direct_register = 1;
			code_table[I].c.destination_direct_register = 1;
		}
	}

	/* If there is only one operand, allocate one more word in memory*/
	else if (operand_source == ' ' && operand_destination != ' ')
	{
		temp = (machine_word*)malloc(sizeof(machine_word));
		if (!temp)
		{
			printf("ERORR!! Memory allocation faild\n");
			error_flag = ON;
			return;
		}
		temp->c.next = NULL;
		code_table[I].c.next = temp;
		code_table[I].c.role = 4; /*Its absolute*/
		code_table[I].c.address = IC; /*Give address*/
		IC++;
		code_table[I].c.next->c.address = IC;
		IC++;
		code_table[I].c.op_code = opcode;
		if (operand_destination == '0')
			code_table[I].c.destination_immidiate = 1;
		else if (operand_destination == '1')
			code_table[I].c.destination_direct = 1;
		else if (operand_destination == '2')
			code_table[I].c.destination_indirect_register = 1;
		else
			code_table[I].c.destination_direct_register = 1;
	}
	/*If there are no operands at all, do not allocate memory words*/
	else if ((operand_source == ' ' && operand_destination == ' '))
	{
		code_table[I].c.role = 4; /*Its absolute*/
		code_table[I].c.address = IC; /*Give address*/
		IC++;
		code_table[I].c.op_code = opcode;
		code_table[I].c.next = NULL;
	}
	else
	{
		/*Allocate two more memmory words*/
		temp = (machine_word*)malloc(sizeof(machine_word));
		if (!temp)
		{
			printf("ERORR!!Memory allocation faild\n");
			error_flag = ON;
			return;
		}
		temp->c.next = NULL;
		code_table[I].c.next = temp;
		temp = (machine_word*)malloc(sizeof(machine_word));
		if (!temp)
		{
			printf("ERORR!! Memory allocation faild\n");
			error_flag = ON;
			return;
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
		if (operand_destination == '0')
			code_table[I].c.destination_immidiate = 1;
		else if (operand_destination == '1')
			code_table[I].c.destination_direct = 1;
		else if (operand_destination == '2')
			code_table[I].c.destination_indirect_register = 1;
		else
			code_table[I].c.destination_direct_register = 1;
		/*For source*/
		if (operand_source == '0')
			code_table[I].c.source_immidiate = 1;
		else if (operand_source == '1')
			code_table[I].c.source_direct = 1;
		else if (operand_source == '2')
			code_table[I].c.source_indirect_register = 1;
		else
			code_table[I].c.source_direct_register = 1;
	}
	I++;
}

/*This function handles the external guides*/
void ext(char* li)
{
	symbol* temp;
	int i;
	li = delete_first_spaces(li);
	if (*li == '\0')
	{
		printf("ERROR!! line %d: Label is missing\n", line_counter);
		error_flag = ON;
	}
	for (i = 0; li[i] != '\0' && li[i] != '\n' && li[i] != ' '; i++);
	if (i > MAX_LABEL_LENGTH)
	{
		printf("ERROR!! line %d: Label is too long\n", line_counter);
		error_flag = ON;
	}
	temp = (symbol*)malloc(sizeof(symbol));
	if (!temp)
	{
		printf("ERORR!! Memory allocation faild\n");
		error_flag = ON;
		return;
	}
	temp->next = head_symbol;
	head_symbol = temp;
	clean_label_name(head_symbol->symbol_name);
	strncpy(head_symbol->symbol_name, li, i);
	head_symbol->is_external = YES;
	head_symbol->address = 0;
}

/*A functions to fill the data table*/
void insert_numerical_data(char* li)
{
	/*For numbers*/
	/* Temp array of ints for storing the input numbers */
	int a[MAX_LINE_LENGTH];
	/* Temp array of chars for storing the input numbers */
	char b[MAX_LINE_LENGTH];
	int i = 0, j, z, counter;
	data_word* temp;
	/* Removing spaces */
	li = delete_first_spaces(li);
	/* Analize, fill the auxiliary array of chars b and give relevant massages */
	if (*li == '\0')
	{
		printf("ERROR!! line %d: Missing parameters\n", line_counter);
		error_flag = ON;
	}
	while (*li != '\0')
	{
		if (*li == '-')
		{
			b[i] = '-';
			i++;
			li++;
			if (!(*(li) > 47 && *(li) < 58))
			{
				printf("ERROR!! line %d: Invalid parameter\n", line_counter);
				error_flag = ON;
			}
		}
		else if (*li == '+')
			li++;
		li = delete_first_spaces(li);
		while ((*li > 47 && *li < 58))
		{
			b[i] = *li;
			i++;
			li++;
		}
		li = delete_first_spaces(li);
		if (*li == ',')
		{
			b[i] = ',';
			i++;
			li = delete_first_spaces(li + 1);
			if (*(li + 1) == ',')
			{
				printf("ERROR!! line %d: Multiple number of consecutive commas\n", line_counter);
				error_flag = ON;
			}
		}
		else
		{
			li = delete_first_spaces(li);
			if ((*li > 47 && *li < 58))
			{
				printf("ERROR!! line %d: Missing comma\n", line_counter);
				error_flag = ON;
			}
			li++;
		}
	}
	/* End of while loop */

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
		if (!temp)
		{
			printf("ERROR!! Memory allocation faild\n");
			error_flag = ON;
			return;
		}
		temp->d.next = data_table[D];
		data_table[D] = temp;
		data_table[D]->d.w = a[j];
		data_table[D]->d.address = DC - z;
		z++;
	}
	D++;
}

/*This function gets as a parameter a string that represent a line content and copy  its content it to updation
of the data_table. each character in the string will be inserted into separate cell.*/
void insert_string_data(char* li)
{
	int i = 0;
	int j = 2;
	char* l = delete_first_spaces(li);
	data_word* temp;
	if (l[0] != '"')
	{
		printf("ERROR!! line %d: Syntax error missing\n", line_counter);
		error_flag = ON;
	}
	while (l[i] != '\0' && l[i] != ' ')
	{
		i++;
	}
	if (i > MAX_STRING)
	{
		printf("ERROR!! line %d: The string is too long, has more than 75 chars\n", line_counter);
		error_flag = ON;
	}
	if (l[i - 2] != '"')
	{
		printf("ERROR!! line %d: Syntax error missing\n", line_counter);
		error_flag = ON;
	}
	DC += i - 2;
	/*If its leggal string*/
	if (i > 3)
		i = i - 3;
	temp = (data_word*)malloc(sizeof(data_word));
	if (!temp)
	{
		printf("ERROR!! Memory allocation faild\n");
		error_flag = ON;
		return;
	}
	data_table[D] = temp;
	temp->d.next = NULL;
	data_table[D]->d.w = 0;
	data_table[D]->d.address = DC - 1;
	for (i = i; i >= 1; i--)
	{
		temp = (data_word*)malloc(sizeof(data_word));
		if (!temp)
		{
			printf("ERROR!! Memory allocation faild\n");
			error_flag = ON;
			return;
		}
		temp->d.next = data_table[D];
		data_table[D] = temp;
		data_table[D]->d.w = l[i];
		data_table[D]->d.address = DC - j;
		j++;
	}
	D++;
}