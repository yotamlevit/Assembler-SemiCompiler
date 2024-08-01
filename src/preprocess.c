//
// Created by Yotam Levit on 24/07/2024.
//

#include "../include/preprocess.h"
#include "../include/hash_map.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

#include "../include/logger.h"
#include "globals.h"


#define MACRO_START "macr"
#define MACRO_END "endmacr\n"
#define EOS '\0'

#define REMOVE_NEW_LINE(str) *strchr(str, '\n') = EOS


boolean is_macro_definition(char* pos) {
    return strncmp(pos, MACRO_START, strlen(MACRO_START)) == 0;
}

int count_macro_occurrences(FILE* file) {
    char buffer[MAX_LINE_LENGTH];
    char* pos;
    int count = 0;

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        pos = delete_first_spaces(buffer);
        strtok(pos, " ");

        if (is_macro_definition(buffer))
            count++;
    }

    return count;
}

HashMapPtr init_macro_hash_map(FILE* file) {
    return createHashMap(count_macro_occurrences(file), NULL, NULL);
}


void *handle_malloc(long object_size) {
    void *object_ptr = malloc(object_size);
    if (object_ptr == NULL) {
        error_log("ERROR_CODE_1");
    }
    return object_ptr;
}

boolean valid_mcro_decl(char *str, char **name, int line_count) {
    /* assumes "mcro " has been encountered right before the function was called */
    char *temp_name, *extra;

    temp_name = *name;//strtok(NULL, " \n");
    if (temp_name == NULL) {
        error_log("ERROR_CODE_9 %s", *name);
        return NO;
    }

    if (is_operation(temp_name) != -1 || is_stop(temp_name) != -1 || is_register(temp_name) == YES) {
        error_log("Macro name is not valid on line %d in file %s", line_count);
        return NO;
    }

    extra = strtok(NULL, "\n");
    if (extra != NULL) {
        error_log("ERROR_CODE_10 Extra params in macro definition on line %d - %s", line_count, extra);
        return NO;
    }

    /* Copy the extracted macro name into a dynamically allocated string */
    *name = handle_malloc((strlen(temp_name) + 1) * sizeof(char));
    strcpy(*name, temp_name);

    return YES;
}


char *copy_text(FILE *fp, fpos_t *pos, int length) {
    /* the function assumes that pos + length < end. this was checked by save_mcro_content function */
    int i;
    char *str;
    //if (fsetpos(fp, pos) != 0) {
    //    printf("fsetpos in copy_text failed\n");
    //    return NULL;
    //}
    str = handle_malloc((length + 1) * sizeof(char));
    for (i = 0; i < length; i++) {
        *(str + i) = getc(fp);
    }
    *(str + i) = EOS;
    //fgetpos(fp, pos);
    return str;
}


char *save_mcro_content(FILE *fp, fpos_t *pos, int *line_count) {
    int mcro_length;
    char *mcro;
    char str[MAX_LINE_LENGTH];
    fpos_t end_pos;

    mcro_length = 0;
    str[0] = EOS;

    /* Read lines from the file until "endmacr" is encountered */
    while (fgets(str, MAX_LINE_LENGTH, fp) && (strcmp(str, MACRO_END)) != 0) {
        (*line_count)++;
        if (strcmp(str, MACRO_END) != 0) {
            mcro_length += strlen(str);
        }
    }

    /* Save the macro end position */
    fgetpos(fp, &end_pos);
    fsetpos(fp, pos);

    /* Copy the macro content into a dynamically allocated string */
    mcro = copy_text(fp, pos, mcro_length);

    /* Return to macro end */
    fsetpos(fp, &end_pos);
    return mcro;
}

boolean add_macro_to_map(FILE* file, HashMapPtr macro_map, char* macro_name, int line_count) {
    boolean result = YES;
    char *content;
    fpos_t pos;
    char* test_macro;

    /* Save the macro content starting from the current file position */
    fgetpos(file, &pos);
    content = save_mcro_content(file, &pos, &line_count);
    if (content == NULL) {
        result = NO;
    }
    else {
        hashMapInsert(macro_map, macro_name, content);
        if(hashMapFind(macro_map, macro_name) == NULL)
        {
            error_log("Failed to add macro %s from line %d", macro_name, line);
            result = NO;
        }
    }

    return result;
}


boolean process_macro_file(FILE* file, HashMapPtr macro_map, char* asm_filename) {
    /* assumes "mcro " has been encountered right before the function was called */
    boolean result = YES;
    FILE *asm_file;
    char buffer[MAX_LINE_LENGTH], temp_buffer[MAX_LINE_LENGTH], original_line[MAX_LINE_LENGTH];
    char* pos, *tmp;
    int count = 0, line_count = 0;
    const int word_len = strlen(MACRO_START);


    asm_file = open_file(asm_filename, "w");

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        line_count++;
        strcpy(temp_buffer, buffer);
        strcpy(original_line, buffer);
        pos = delete_first_spaces(temp_buffer);
        pos = strtok(pos, " ");

        if (is_macro_definition(temp_buffer)) {
            pos = strtok(NULL, " \n");
            if (!valid_mcro_decl(delete_first_spaces(temp_buffer), &pos, line_count))
                result = NO;
            else
                result = result && add_macro_to_map(file, macro_map, pos, line_count);

            //process_macro(pos, macro_map);
        }
        else {
            pos = delete_first_spaces(buffer);
            REMOVE_NEW_LINE(pos);
            char* macro_content = NULL;

            if ( (macro_content = (char *)hashMapFind(macro_map, pos)) != NULL)
            {
                if (fprintf(asm_file, "%s", macro_content) < 0) {
                    // Handle error if the write fails
                    perror("Error writing to file");
                    fclose(file);
                }
                printf("%s\n", macro_content);
                continue;
            }

            while((pos = strtok(NULL, " ")) != NULL)
            {
                if ( (macro_content = (char *)hashMapFind(macro_map, pos)) != NULL) {
                    if (fprintf(asm_file, "%s", delete_first_spaces(macro_content)) < 0) {
                        // Handle error if the write fails
                        perror("Error writing to file");
                        fclose(file);
                    }
                }
            }

            if (macro_content == NULL) {
                if (*original_line != '\n' &&  fprintf(asm_file, "%s", original_line) < 0) {
                    // Handle error if the write fails
                    perror("Error writing to file");
                    fclose(file);
                }
            }

        }
        /* If the tocken is a macro */


        pos = buffer;
        while ((pos = strstr(pos, MACRO_START)) != NULL) {
            count++;
            pos += word_len;
        }
    }

    fclose(asm_file);
    return result;

}

/* Chack macro name
 * validate macro declare
 */

int macro_exec(FILE* file, char* filename) {
    boolean result = YES;
    rewind(file);

    HashMapPtr macro_map = init_macro_hash_map(file);

    if (!macro_map)
        return NO;
    rewind(file);

    add_file_name_extension(filename, "asm");
    result = process_macro_file(file, macro_map, filename);

    return result;
}