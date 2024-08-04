#include "../include/utils.h"
#include "../include/validators.h"
#include "../include/tables.h"
#include "../include/first_pass.h"

#include <stdbool.h>

#include "../include/constants_tables.h"
#include "globals.h"
#include "logger.h"

/*Operation code.Reliable only when the action is valid*/
int opcode;


boolean analyze_input_line(char* asm_line)
{
	asm_line = delete_first_spaces(asm_line);
	if (!strncmp(asm_line, EXTERN_LABEL, strlen(EXTERN_LABEL)))
		return ext(asm_line + strlen(EXTERN_LABEL));
	if (!strncmp(asm_line, DATA_LABEL, strlen(DATA_LABEL)))
		return insert_numerical_data(asm_line + strlen(DATA_LABEL));
	if (!strncmp(asm_line, STRING_LABEL, strlen(STRING_LABEL)))
		return insert_string_data(asm_line + strlen(STRING_LABEL));
	if(*asm_line == ';' || *asm_line == '\0' || *asm_line == '\n' || (!strncmp(asm_line, ENTRY_LABEL, strlen(ENTRY_LABEL))))
		return TRUE;
	opcode = is_operation(asm_line);
    if (opcode != -1) // In second pass refactor i remove opcode global from the function
	{
		operation(asm_line + 3);
		return TRUE;
	}
	opcode = is_stop(asm_line);
	if (opcode != -1)/*if it is stop operation*/ // In second pass refactor i remove opcode global from the function
	{
		operation(asm_line + 4);
		return TRUE;
	}
	if (is_label(asm_line))
		return label_actions(asm_line);

	error_log("line %d: The command was not found\n", line_counter);
	return FALSE;
}

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

boolean first_pass_exec(FILE* file_handle)
{
	boolean status = TRUE, analyze_input_line_result;
    line_counter = 0;
    while (!feof(file_handle))
    {
        analyze_input_line_result = analyze_input_line(line);
    	if (!analyze_input_line_result)
    		status = FALSE;
        line_counter++;
        fgets(line, MAX_LINE_LENGTH, file_handle);
    }
    validate_memory(IC, DC);
	return status;
}

/**
 * @brief Checks if a given line contains a label.
 *
 * The is_label function processes a given line to determine if it contains a label.
 * It removes leading spaces and then checks for the presence of a colon (':') which
 * signifies a label in the line. The function scans up to the maximum line length.
 *
 * @param asm_line A pointer to the line to be checked for a label.
 * @return A boolean value indicating whether the line contains a label.
 *         Returns TRUE if the line contains a label. Otherwise, returns FALSE.
 */
boolean is_label(char* asm_line)
{
	int i;
	asm_line = delete_first_spaces(asm_line);
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		if (asm_line[i] == ':')
			return TRUE;
	}
	return FALSE;
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
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the label is processed successfully. Otherwise, returns FALSE.
 */
boolean label_actions(char* asm_line)
{
	char* p;
	symbol* temp;
	int i;
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		if (asm_line[i] == ':')
		{
			if (i > MAX_LABEL_LENGTH)
			{
				error_log("line %d: Lable is too long, has more than 30 chars\n", line_counter);
				return FALSE;
			}
			else
			{
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
			}
			p = (asm_line + i + 1);
			p = delete_first_spaces(p);
			if (*p == '.')
			{
				/*If its guide statement*/
				head_symbol->address = DC;
				head_symbol->is_attached_directive = TRUE;
				head_symbol->is_external = FALSE;
				if (!strncmp(p, ENTRY_LABEL, strlen(ENTRY_LABEL)))
					warning_log("line %d: A label defined at the beginig of entry statement is ignored\n" ,line_counter);
				else if (!strncmp(p, EXTERN_LABEL, strlen(EXTERN_LABEL)))
					warning_log("line %d: A label defined at the beginig of extern statement is ignored.\n", line_counter);
				else
					/*Sends again to analize to find out if its string or data*/
					return analyze_input_line(p);
			}
			else
			{
				/*If this is a statement of instruction*/
				head_symbol->address = IC;
				head_symbol->is_attached_directive = FALSE;
				head_symbol->is_external = FALSE;
				/*Go again to analize to find out which instruction statement*/
				return analyze_input_line(p);
			}
		}
	}
	return TRUE;
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
	boolean is_source = FALSE;
	boolean is_destination = FALSE;
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
			is_source = TRUE;
		i++;
	}
	i = 0;
	while ((operation_mode[opcode][0])[i] != '\0')
	{
		/*Checks whether the specific OP supports the destination add method*/
		if ((operation_mode[opcode][0])[i] == operand_destination)
			is_destination = TRUE;
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

/**
 * @brief Processes a line to check and handle an external label.
 *
 * The ext function processes a given line to determine if it contains a valid external
 * label. It removes leading spaces, checks for an empty line, validates the label's length,
 * and adds the label to the symbol table if valid. Errors are logged if the line is
 * empty, the label is too long, or memory allocation fails.
 *
 * @param asm_line A pointer to the line to be processed.
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the line contains a valid external label and is processed
 *         successfully. Otherwise, returns FALSE.
 */
boolean ext(char* asm_line)
{
	symbol* temp;
	int i = 0;

	asm_line = delete_first_spaces(asm_line);
	if (*asm_line == '\0' || *asm_line == '\n') {
		error_log("line %d: Label is missing", line_counter);
		return FALSE;
	}

	while (asm_line[i] != '\0' && asm_line[i] != '\n' && asm_line[i] != ' ') { i++; }
	if (i > MAX_LABEL_LENGTH)
	{
		error_log("line %d: Label is too long", line_counter);
		return FALSE;
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
	return TRUE;
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
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the numerical data is successfully processed and inserted
 *         into the data table. Otherwise, returns FALSE.
 */
boolean insert_numerical_data(char* asm_line)
{
	/* TODO: Split logic to multiple functions, Fix memory leak */
	int a[MAX_LINE_LENGTH];
	char b[MAX_LINE_LENGTH];
	int i = 0, j, z, counter;
	data_word* temp;
	asm_line = delete_first_spaces(asm_line);
	if (*asm_line == '\0' || *asm_line == '\n')
		error_log("line %d: Missing parameters", line_counter);
	while (*asm_line != '\0')
	{
		if (*asm_line == '-')
		{
			b[i] = '-';
			i++;
			asm_line++;
			if (!(*(asm_line) > 47 && *(asm_line) < 58))
			{
				error_log("line %d: Invalid parameter", line_counter);
				return FALSE;
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
				error_log("line %d: Multiple number of consecutive commas\n", line_counter);
				return FALSE;
			}
		}
		else
		{
			asm_line = delete_first_spaces(asm_line);
			if ((*asm_line > 47 && *asm_line < 58))
			{
				error_log("line %d: Missing comma\n", line_counter);
				return FALSE;
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
	return TRUE;
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
 * @return A boolean value indicating the success of the operation.
 *         Returns TRUE if the string data is successfully processed and inserted
 *         into the data table. Otherwise, returns FALSE.
 */
boolean insert_string_data(char* asm_line)
{
	int i = 0, j = 2, k;
	data_word* temp;

	asm_line = delete_first_spaces(asm_line);
	if (asm_line[0] != '"')
	{
		error_log("line %d: Syntax error missing", line_counter);
		return FALSE;
	}

	while (asm_line[i] != '\0' && asm_line[i] != ' ') i++;
	if (i > MAX_STRING)
	{
		error_log("line %d: The string is too long, has more than 75 chars", line_counter);
		return FALSE;
	}

	if (asm_line[i - 2] != '"')
	{
		error_log("line %d: Syntax error missing", line_counter);
		return FALSE;
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
	return TRUE;
}