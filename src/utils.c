//
// Created by Yotam Levit on 19/07/2024.
//

#include "../include/globals.h"
#include "../include/constants_tables.h"
#include "../include/first_pass.h"
#include "../include/auxiliary.h"
#include "../include/validators.h"
#include "../include/second_pass.h"
#include "../include/utils.h"


/*Operation code.Reliable only when the action is valid*/
int opcode;


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
int is_operation(char* li)
{
	int i;
 	for(i = 0;i<OPERATIONS_AMOUNT -1; i++)
	{
		/*Without stop op*/
		if (!(strncmp(li, operation_mode[i][2], 3)))
		{
			/*If the operation exist return true--1*/
			opcode = i;
			return 1;
		}
	}
	return 0;
}

/*This function checks if it is stop operation*/
int is_stop(char* line)
{
	if (!(strncmp(line, "stop", 4)))
	{
		opcode = 15;
		return 1;
	}
	return 0;
}