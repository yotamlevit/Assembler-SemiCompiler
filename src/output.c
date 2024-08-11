
#include <stdio.h>

#include "../include/tables.h"
#include "../include/output.h"
#include "../include/logger.h"
#include "../include/utils.h"

extern char* file_name;

/*Definitions: */
/*index of code_table, index of data_table, instruction counter, data counter.*/
int I, D;
symbol* head_symbol, *head_entries, *head_externals;
data_word* data_table[150]; /*data array*/
code_word code_table[150];/*code array*/


/*End of create_external_file function*//*This function creates an object file that contains the machine code translating to octal base with the appropriate
memmory addres in decimal base (code divided to 3 bits every time as an octal number). */
void create_object_file(char* file_name)
{
	int i;
	machine_word* temp;
	data_word* tempD;
	FILE* fd;
	add_file_name_extension(file_name, "ob");
	fd = fopen(file_name, "w");
	if (!fd)
	{
		error_log("Cannot create object file");
		return;
	}
	fprintf(fd, "   %d %d\n", IC - IC_INITIAL_VALUE, DC);/*fprint IC and DC*/
	for (i = 0; i < I; i++)
	{
		fprintf(fd, " %d  %o", code_table[i].c.address, code_table[i].parts.w5);
		fprintf(fd, "%o", code_table[i].parts.w4);
		fprintf(fd, "%o", code_table[i].parts.w3);
		fprintf(fd, "%o", code_table[i].parts.w2);
		fprintf(fd, "%o\n", code_table[i].parts.w1);
		temp = code_table[i].c.next;
		while (temp)
		{
			fprintf(fd, " %d  %o", temp->c.address, temp->parts.w5);
			fprintf(fd, "%o", temp->parts.w4);
			fprintf(fd, "%o", temp->parts.w3);
			fprintf(fd, "%o", temp->parts.w2);
			fprintf(fd, "%o\n", temp->parts.w1);
			temp = temp->c.next;
		}
	}/*End of for loop of code_table*/
	for (i = 0; i < D; i++)
	{
		tempD = data_table[i];
		while (tempD)
		{
			fprintf(fd, " %d  %o", tempD->d.address + IC, tempD->parts.w5);
			fprintf(fd, "%o", tempD->parts.w4);
			fprintf(fd, "%o", tempD->parts.w3);
			fprintf(fd, "%o", tempD->parts.w2);
			fprintf(fd, "%o\n", tempD->parts.w1);
			tempD = tempD->d.next;
		}
	}
	fclose(fd);
}


/*This function creates an enteranl file that contain the labels that defined in the assembly text as an entry label
with it's appropriate value in the labels table.*/
void create_entry_file(char* file_name)
{
	symbol* entry_symbol = head_entries;
	FILE* fd;
	add_file_name_extension(file_name, "ent");
	if (!entry_symbol)
		return;
	fd = fopen(file_name, "w");
	if (!fd)
	{
		error_log("Cannot create %s file", file_name);
		return;
	}
	while (entry_symbol)
	{
		fprintf(fd, " %s  %d\n", entry_symbol->symbol_name, entry_symbol->address);
		entry_symbol = entry_symbol->next;
	}
	fclose(fd);
}


/*This function creates an external file that contain the labels that defined as externals and the appropriate
address that they have been used in the asseembly program.*/
void create_external_file(char* file_name)
{
	symbol* entry_symbol = head_externals;
	FILE* fd;
	add_file_name_extension(file_name, "ext");
	if (!entry_symbol)
		return;
	fd = fopen(file_name, "w");
	if (!fd)
	{
		error_log("Cannot create %s file", file_name);
		return;
	}
	while (entry_symbol)
	{
		fprintf(fd, " %s %d\n", entry_symbol->symbol_name, entry_symbol->address);
		entry_symbol = entry_symbol->next;
	}
	fclose(fd);
}


void create_output_files(char* file_name, int* line_index)
{
    I = *line_index;
    create_object_file(file_name);
    create_entry_file(file_name);
    create_external_file(file_name);
}