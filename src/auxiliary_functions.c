#include "../include/auxiliary.h"
#include "../include/globals.h"

extern symbol* head_symbol;
extern symbol* head_entries;
extern symbol* head_externals;

extern data_word* data_table[TABLE_SIZE];
extern code_word code_table[TABLE_SIZE];
extern int D;
extern char* file_name;


/*This function gets a string that represent the current line content and find a comma in it. It retruns the comma index if any.*/
char* find_comma(char* line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++)
	{
		if (line[i] == ',')
			break;
	}
	return line + i;
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

/*This function gets a string that represent a line content and initalizes it.*/
void clean_line(char* line)
{
	int i;
	for (i = 0; i < MAX_LINE_LENGTH; i++)
	{
		line[i] = '\0';
	}
}

/*This function gets a string that represent a label content and initalizes it.*/
void clean_label_name(char* label)
{
	int i;
	for (i = 0; i < MAX_LABEL_LENGTH; i++)
	{
		label[i] = '\0';
	}
}

