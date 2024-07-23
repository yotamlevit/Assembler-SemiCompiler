#include "../include/auxiliary.h"
#include "../include/globals.h"
#include "utils.h"

extern symbol* head_symbol;
extern symbol* head_entries;
extern symbol* head_externals;

extern data_word* data_table[TABLE_SIZE];
extern code_word code_table[TABLE_SIZE];
extern int D;
extern char* file_name;


void free_data_table()
{
	int i;
	data_word* temp;
	data_word* temp2;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		temp = data_table[i];
		temp2 = temp;
		while (temp2)
		{
			temp = temp2;
			temp2 = temp2->d.next;
			free(temp);
		}
		data_table[i] = NULL;
	}
}

void free_code_table()
{
	int i;
	machine_word* temp;
	machine_word* temp2;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		temp = code_table[i].c.next;
		temp2 = temp;
		while (temp2)
		{
			temp = temp2;
			temp2 = temp2->c.next;
			free(temp);
		}
		code_table[i].parts.w1 = 0;
		code_table[i].parts.w2 = 0;
		code_table[i].parts.w3 = 0;
		code_table[i].parts.w4 = 0;
		code_table[i].parts.w5 = 0;
		code_table[i].c.next = NULL;
	}
}
/*The following functions release dynamic memory using a temporary pointer to the table: */

/*This function frees the dynamic memory for the symbol table */
void free_symbol_table()
{
	symbol* temp = head_symbol;
	symbol* temp2 = temp;
	while (temp2)
	{
		temp = temp2;
		temp2 = temp2->next;
		free(temp);
	}
	head_symbol = NULL;
}

/*This function frees the dynamic memory for the entries list*/
void free_entries_list()
{
	symbol* temp = head_entries;
	symbol* temp2 = temp;
	while (temp2)
	{
		temp = temp2;
		temp2 = temp2->next;
		free(temp);
	}
	head_entries = NULL;
}

/*This function frees the dynamic memory for the externals list*/
void free_externals_list()
{
	symbol* temp = head_externals;
	symbol* temp2 = temp;
	while (temp2)
	{
		temp = temp2;
		temp2 = temp2->next;
		free(temp);
	}
	head_externals = NULL;
}



