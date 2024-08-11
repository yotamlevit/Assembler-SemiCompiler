
#include <stdio.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/preprocess.h"
#include "../include/utils.h"

#define MACRO_START "macr"
#define MACRO_END "endmacr\n"
#define EOS '\0'
#define NEW_LINE '\n'
#define ASM_COMMENT ';'
#define MACRO_OUTPUT_EXTENSION "asm"
#define SPACE " "
#define FILE_WRITE_MODE "w"
#define REMOVE_NEW_LINE(str) *strchr(str, NEW_LINE) = EOS

/**
 * Checks if a given position in a string is the start of a macro definition.
 *
 * @param pos The position in the string to check.
 * @return A boolean indicating if the position is the start of a macro definition.
 */
boolean is_macro_definition(char* pos) {
    return strncmp(pos, MACRO_START, strlen(MACRO_START)) == 0;
}


/**
 * Counts the number of macro occurrences in a file.
 *
 * @param file The file to count macro occurrences in.
 * @return The number of macro occurrences.
 */
int count_macro_occurrences(FILE* file) {
    char buffer[MAX_LINE_LENGTH];
    char* pos;
    int count = 0;

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        pos = delete_first_spaces(buffer);
        strtok(pos, SPACE);

        if (is_macro_definition(buffer))
            count++;
    }

    return count;
}


/**
 * Initializes a hash map for storing macros.
 *
 * @param file The file to read macros from.
 * @return A pointer to the initialized hash map.
 */
HashMapPtr init_macro_hash_map(FILE* file) {
    return createHashMap(count_macro_occurrences(file), NULL, NULL);
}


/**
 * Allocates memory for an object of a given size.
 *
 * @param object_size The size of the object to allocate memory for.
 * @return A pointer to the allocated memory.
 */
void *handle_malloc(long object_size) {
    void *object_ptr = malloc(object_size);
    if (object_ptr == NULL) {
        error_log("ERROR_CODE_1");
    }
    return object_ptr;
}


/**
 * Validates a macro definition.
 *
 * @param name A pointer to the name of the macro.
 * @param line_count The current line number in the file.
 * @return A boolean indicating if the macro definition is valid.
 */
boolean is_valid_macro_definition(char **name, int line_count) {
    /* assumes "mcro " has been encountered right before the function was called */
    char *temp_name, *extra;

    temp_name = *name;
    if (temp_name == NULL) {
        error_log("A macro was declared without name definition %s", *name);
        return FALSE;
    }

    if (is_operation(temp_name) != -1 || is_stop(temp_name) != -1 || is_register(temp_name) == TRUE) {
        error_log("Macro name is not valid on line %d in file %s", line_count);
        return FALSE;
    }

    extra = strtok(NULL, "\n");
    if (extra != NULL) {
        error_log("Extra params in macro definition on line %d - %s", line_count, extra);
        return FALSE;
    }

    /* Copy the extracted macro name into a dynamically allocated string */
    *name = handle_malloc((strlen(temp_name) + 1) * sizeof(char));
    strcpy(*name, temp_name);

    return TRUE;
}


/**
 * Copies the content of a macro from a file.
 *
 * @param fp The file pointer to read from.
 * @param length The length of the macro content.
 * @return A pointer to the copied macro content.
 */
char *copy_macro_content(FILE *fp, int length) {
    /* the function assumes that pos + length < end */
    int i;
    char *str;

    str = handle_malloc((length + 1) * sizeof(char));
    for (i = 0; i < length; i++) {
        *(str + i) = getc(fp);
    }
    *(str + i) = EOS;

    return str;
}


/**
 * Saves the content of a macro from a file.
 *
 * @param fp The file pointer to read from.
 * @param pos The current position in the file.
 * @param line_count A pointer to the current line number in the file.
 * @return A pointer to the saved macro content.
 */
char *save_macro_content(FILE *fp, fpos_t *pos, int *line_count) {
    int macro_length;
    char *macro_content;
    char str[MAX_LINE_LENGTH];
    fpos_t end_pos;

    macro_length = 0;
    str[0] = EOS;

    /* Read lines from the file until MACRO_END is found */
    while (fgets(str, MAX_LINE_LENGTH, fp) && (strcmp(str, MACRO_END)) != 0) {
        (*line_count)++;
        if (strcmp(str, MACRO_END) != 0) {
            macro_length += strlen(str);
        }
    }

    /* Save the macro end position */
    fgetpos(fp, &end_pos);
    fsetpos(fp, pos);

    /* Copy the macro content into a dynamically allocated string */
    macro_content = copy_macro_content(fp, macro_length);

    /* Return to macro end */
    fsetpos(fp, &end_pos);
    return macro_content;
}


/**
 * Adds a macro to the hash map.
 *
 * @param file The file pointer to read from.
 * @param macro_map The hash map to add the macro to.
 * @param macro_name The name of the macro.
 * @param line_count The current line number in the file.
 * @return A boolean indicating if the macro was successfully added to the hash map.
 */
boolean add_macro_to_map(FILE* file, HashMapPtr macro_map, char* macro_name, int line_count) {
    boolean result = TRUE;
    char *content;
    fpos_t pos;

    /* Save the macro content starting from the current file position */
    fgetpos(file, &pos);
    content = save_macro_content(file, &pos, &line_count);
    if (content == NULL) {
        result = FALSE;
    }
    else {
        hashMapInsert(macro_map, macro_name, content);
        if(hashMapFind(macro_map, macro_name) == NULL)
        {
            error_log("Failed to add macro %s from line %d", macro_name, line);
            result = FALSE;
        }
    }

    return result;
}


/**
 * Handles a new macro definition in the file.
 *
 * @param file The file pointer to read from.
 * @param macro_map The hash map to add the macro to.
 * @param macro_name The name of the macro.
 * @param line_count The current line number in the file.
 * @return A boolean indicating if the new macro was successfully handled.
 */
boolean handle_new_macro(FILE* file, HashMapPtr macro_map, char* macro_name, int line_count) {
    boolean result = TRUE;

    macro_name = strtok(NULL, " \n");
    if (!is_valid_macro_definition(&macro_name, line_count))
        result = FALSE;
    else if ((hashMapFind(macro_map, macro_name)) != NULL)
    {
        error_log("Duplicate macro definition on line %d - %s", line_count, macro_name);
        result = FALSE;
    }
    else
        result = add_macro_to_map(file, macro_map, macro_name, line_count);

    return result;
}


/**
 * Handles a non-new macro line in the file.
 *
 * @param file The file pointer to read from.
 * @param pos The current position in the string.
 * @param original_line The original line from the file.
 * @param macro_map The hash map containing macros.
 * @param line_count The current line number in the file.
 * @return A boolean indicating if the non-new macro line was successfully handled.
 */
boolean handle_non_new_macro_line(FILE* file, char* pos, char* original_line, HashMapPtr macro_map, int line_count) {
    char* macro_content = NULL;

    if(ends_with_newline(pos)) {
        REMOVE_NEW_LINE(pos);
    }

    do{
        /* Check if the tokenized part of the line matches a macro name in the hash map */
        if ( (macro_content = (char *)hashMapFind(macro_map, pos)) != NULL)
        {
            /* If found, write the macro content to the output file */
            if (write_line_to_file(file, macro_content) == FALSE) {
                error_log("Error while writing macro content into output file .asm with line %d", line_count);
                return FALSE;
            }
            return TRUE;
        }
    } while((pos = strtok(NULL, SPACE)) != NULL && macro_content == NULL);


    /* If no macro found, write the original line to the output file */
    if (*original_line != '\n') {
        if (write_line_to_file(file, original_line) == FALSE) {
            error_log("Error while writing original code into output file .asm with line %d", line_count);
            return FALSE;
        }
    }
}


/**
 * Processes a macro file and writes the output to an assembly file.
 * In one iteration the program add all the macros into a hash map and replace the macros in the file.
 *
 * @param file The file pointer to read from.
 * @param macro_map The hash map containing macros.
 * @param asm_filename The name of the output assembly file.
 * @return A boolean indicating if the macro file was successfully processed.
 */
boolean process_macro_file(FILE* file, HashMapPtr macro_map, char* asm_filename) {
    boolean result = TRUE;
    FILE *asm_file;
    char buffer[MAX_LINE_LENGTH], temp_buffer[MAX_LINE_LENGTH], original_line[MAX_LINE_LENGTH];
    char* pos;
    int line_count = 0;


    asm_file = open_file(asm_filename, FILE_WRITE_MODE);

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        line_count++;
        strcpy(temp_buffer, buffer);
        strcpy(original_line, buffer);
        pos = delete_first_spaces(temp_buffer);
        pos = strtok(pos, SPACE);

        if (is_macro_definition(temp_buffer)) {
            result = result && handle_new_macro(file, macro_map, pos, line_count);
        }
        else {
            pos = delete_first_spaces(buffer);
            if (*pos != ASM_COMMENT && *pos != EOS && *pos != NEW_LINE)
                result = result && handle_non_new_macro_line(asm_file, pos, original_line, macro_map, line_count);
        }
    }

    fclose(asm_file);
    return result;
}


/**
 * Executes the macro processing on a given file.
 *
 * @param file The file pointer to read from.
 * @param filename The name of the file to process.
 * @return An integer indicating success (1) or failure (0).
 */
int macro_exec(FILE* file, char* filename, HashMapPtr* macro_map) {
    boolean result;
    rewind(file);

    *macro_map = init_macro_hash_map(file);

    if (!*macro_map)
    {
        error_log("Failed to initialize macro hash map");
        return FALSE;
    }

    rewind(file);

    add_file_name_extension(filename, MACRO_OUTPUT_EXTENSION);
    result = process_macro_file(file, *macro_map, filename);
    return result;
}