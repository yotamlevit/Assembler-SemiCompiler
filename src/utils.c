
#include <string.h>
#include <stdio.h>

#include "../include/logger.h"
#include "../include/globals.h"
#include "../include/constants_tables.h"
#include "../include/first_pass.h"
#include "../include/utils.h"


#define MAX_BUFFER_COUNT 5
#define APPEND_FILE_PATH "a"


FILE* open_file(char* file, char* mode)
{
    FILE* fp = fopen(file, mode);
    if (fp == NULL)
    {
        error_log("Could not open file: %s", file);
        return NULL;
    }
    return fp;
}


/**
 * @brief Checks if a given assembly line contains a valid operation.
 *
 * The is_operation function processes a given assembly line to determine if it contains
 * a valid operation. It compares the line against a predefined list of operations,
 * excluding the "stop" operation. If the operation exists, the function returns its opcode.
 *
 * @param asm_line A pointer to the assembly line to be checked for an operation.
 * @return An integer representing the opcode of the operation if it exists.
 *         Returns the opcode of the operation if found. Otherwise, returns -1.
 */
int is_operation(char* asm_line)
{
	int i;
 	for(i = 0; i<OPERATIONS_AMOUNT - 1; i++)
	{
		/*Without stop op*/
		if (!(strncmp(asm_line, operation_mode[i][2], OPERATION_LENGTH)))
		{
			/*If the operation exist return the opcodde */
			return i;
		}
	}
	return -1;
}


/*This function checks if it is stop operation*/
int is_stop(char* line)
{
	if (!(strncmp(line, "stop", STOP_LENGTH)))
	{
		return  STOP_OPCODE;
	}
	return -1;
}


char* find_next_symbol_in_str(char* search_line, char symbol)
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


void add_file_name_extension(char* filename,char* extension)
{
    char* dot = find_next_symbol_in_str(filename, '.');
    strcpy(dot+1, extension);
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


boolean is_register(char* str) {

    return *str == REGISTER_SYMBOL && (*(str + 1) >= '0' && *(str + 1) <= '7');
}


boolean write_line_to_file(FILE* fp, char* line) {
    if (fprintf(fp, "%s", line) < 0) {
        /* Handle error if the write fails */
        fclose(fp);
        return FALSE;
    }
    return TRUE;
}


/**
 * Checks if a string ends with a newline character.
 *
 * @param str The string to check.
 * @return A boolean indicating if the string ends with a newline (true) or not (false).
 */
boolean ends_with_newline(const char *str) {
    size_t len = strlen(str);
    return len > 0 && str[len - 1] == '\n';
}


/**
 * Checks if an assembly line contains a label.
 *
 * @param asm_line The assembly line to be checked.
 * @return A boolean indicating if the line contains a label.
 */
boolean is_label(char* asm_line)
{
	return *find_next_symbol_in_str(asm_line, LABEL_SYMBOL) == LABEL_SYMBOL;
}


void write_buffer_to_file(char* filename, char* line) {
    FILE *asm_file;
    int buffer_count = 0, length = strlen(line), chars_to_write;

    while (buffer_count < length) {
        asm_file = open_file(filename, APPEND_FILE_PATH);

        /* Calculate the number of characters to write in this batch */
        chars_to_write = (buffer_count + MAX_BUFFER_COUNT <= length) ? MAX_BUFFER_COUNT : length - buffer_count;

        /* Write the batch to the file  */
        fwrite(&line[buffer_count], sizeof(char), chars_to_write, asm_file);

        /* Close the file */
        fclose(asm_file);

        /* Move to the next batch */
        buffer_count += MAX_BUFFER_COUNT;
    }
}
