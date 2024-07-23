//
// Created by Yotam Levit on 19/07/2024.
//

#include "utils.h"
#include "../include/globals.h"
#include "../include/constants_tables.h"
#include "../include/first_pass.h"
#include "../include/tables.h"
#include "../include/validators.h"
#include "../include/utils.h"


/*Operation code.Reliable only when the action is valid*/
int opcode;
extern char* file_name;


FILE* open_file(char* file)
{
    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("ERROR!! File not found or file with extension.\n");
        return NULL;
    }
    return fp;
}

/*This function checks if it is an action statement*/
int is_operation(char* li, int* opcode) // TODO add stop to this function
{
	int i;
 	for(i = 0;i<OPERATIONS_AMOUNT -1; i++)
	{
		/*Without stop op*/
		if (!(strncmp(li, operation_mode[i][2], OPERATION_LENGTH)))
		{
			/*If the operation exist return true--1*/
			*opcode = i;
			return 1;
		}
	}
	return 0;
}

/*This function checks if it is stop operation*/
int is_stop(char* line, int* opcode)
{
	if (!(strncmp(line, "stop", STOP_LENGTH)))
	{
		*opcode = STOP_OPCODE;
		return 1;
	}
	return 0;
}

char* find_next_symbol_in_line(char* search_line, char symbol)
{
    char* symbol_pos;
    for (symbol_pos = search_line; *symbol_pos != symbol && *symbol_pos != END_OF_STR; symbol_pos++);
    return symbol_pos;
}

/*This function gets a string that represent a line content and skip the first white spaces in it. It return a pointer to the first non white space character. */
char* delete_first_spaces(char* line)
{
	while (*line == ' ' || *line == '\t')
		line++;
	return line;
}

/*This function gets an extension of a file name and add/change extentsion in the global file_name variable.*/
void add_extension_2_file_name(char* extension)
{
	int i;
	for (i = 0; file_name[i] != '.'; i++);
	strcpy((file_name + i), extension);
}

/*This function gets a string that represent a line content and initalizes it.*/
void clean_line(char* line)
{
	int i;
	for (i = 0; i < MAX_LINE_LENGTH; i++)
		line[i] = END_OF_STR;
}

/*This function gets a string that represent a label content and initalizes it.*/
void clean_label_name(char* label)
{
	int i;
	for (i = 0; i < MAX_LABEL_LENGTH; i++)
		label[i] = END_OF_STR;
}