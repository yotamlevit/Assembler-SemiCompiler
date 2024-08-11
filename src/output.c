
#include <stdio.h>

#include "../include/tables.h"
#include "../include/output.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/globals.h"


/*Definitions: */
/*index of code_table, index of data_table, instruction counter, data counter.*/
int I, D;
symbol* head_symbol, *head_entries, *head_externals;
data_word* data_table[150]; /*data array*/
code_word code_table[150];/*code array*/


/**
 * Creates an object file that contains the machine code translated to octal base
 * with the appropriate memory address in decimal base.
 *
 * The code is divided into 3-bit sections and represented as octal numbers.
 *
 * @param file_name The base name of the file to which the object code will be written.
 */
void create_object_file(char* file_name)
{
	int i;
	machine_word* temp;
	data_word* tempD;
	FILE* fd;
	add_file_name_extension(file_name, "ob");
    fd = open_file(file_name, FILE_WRITE_MODE);
	if (fd == NULL)
		return;

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


/**
 * Creates an entry file that contains the labels defined as entry labels
 * in the assembly source with their corresponding addresses.
 *
 * @param file_name The base name of the file to which the entry labels will be written.
 */
void create_entry_file(char* file_name)
{
	symbol* entry_symbol = head_entries;
	FILE* fd;
	add_file_name_extension(file_name, "ent");
	if (!entry_symbol)
		return;
    fd = open_file(file_name, FILE_WRITE_MODE);
    if (fd == NULL)
        return;

	while (entry_symbol)
	{
		fprintf(fd, " %s  %d\n", entry_symbol->symbol_name, entry_symbol->address);
		entry_symbol = entry_symbol->next;
	}
	fclose(fd);
}


/**
 * Creates an external file that contains the labels defined as external labels
 * in the assembly source with their corresponding addresses where they are used.
 *
 * @param file_name The base name of the file to which the external labels will be written.
 */
void create_external_file(char* file_name)
{
	symbol* entry_symbol = head_externals;
	FILE* fd;
	add_file_name_extension(file_name, "ext");
	if (!entry_symbol)
		return;
    fd = open_file(file_name, FILE_WRITE_MODE);
    if (fd == NULL)
        return;

	while (entry_symbol)
	{
		fprintf(fd, " %s %d\n", entry_symbol->symbol_name, entry_symbol->address);
		entry_symbol = entry_symbol->next;
	}
	fclose(fd);
}


/**
 * Creates all necessary output files (object, entry, and external) for the assembly program.
 *
 * @param file_name The base name of the files to be created.
 * @param line_index A pointer to the current line index (instruction counter).
 */
void create_output_files(char* file_name, int* line_index)
{
    I = *line_index;
    create_object_file(file_name);
    create_entry_file(file_name);
    create_external_file(file_name);
}