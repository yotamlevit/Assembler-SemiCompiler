//
// Created by Yotam Levit on 24/07/2024.
//

#include "../include/preprocess.h"
#include "../include/tables.h"
#include "../include/hash_map.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

#include "../include/logger.h"


#define MACRO_START "macr"


boolean is_macro_definition(char* pos) {
    return strncmp(pos, MACRO_START, strlen(MACRO_START)) == 0;
}

int count_macro_occurrences(FILE* file) {
    char buffer[MAX_LINE_LENGTH];
    char* pos;
    int count = 0;
    const int maco_def_len = strlen(MACRO_START);

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


boolean is_register(char* str) {

    return *str == 'r' && (*(str + 1) >= '0' && *(str + 1) <= '7');
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
    if (fsetpos(fp, pos) != 0) {
        printf("fsetpos in copy_text failed\n");
        return NULL;
    }
    str = handle_malloc((length + 1) * sizeof(char));
    for (i = 0; i < length; i++) {
        *(str + i) = getc(fp);
    }
    *(str + i) = '\0';
    fgetpos(fp, pos);
    return str;
}


char *save_mcro_content(FILE *fp, fpos_t *pos, int *line_count) {
    int mcro_length;
    char *mcro;
    char str[MAX_LINE_LENGTH];

    /* Set the file pointer to the provided position */
    if (fsetpos(fp, pos) != 0) {
        error_log("ERROR_CODE_11");
        return NULL;
    }
    mcro_length = 0;
    str[0] = '\0';

    /* Read lines from the file until "endmcro" is encountered */
    while (fgets(str, MAX_LINE_LENGTH, fp) && (strcmp(str, "endmacr\n")) != 0) {
        /* checking for a line with endmcro and extra text */
        if ((strstr(str, "endmacr") != NULL) && strlen(str) != strlen("endmacr")) {
            error_log("ERROR_CODE_12");
            return NULL;
        }
        (*line_count)++;
        if (strcmp(str, "endmacr\n") != 0) {
            mcro_length += strlen(str);
        }
    }

    /* Copy the macro content into a dynamically allocated string */
    mcro = copy_text(fp, pos, mcro_length);
    return mcro;
}

boolean add_macro_to_map(FILE* file, HashMapPtr macro_map, char* macro_name) {
    int line_count, mcro_line;
    boolean result = YES;
    char buffer[MAX_LINE_LENGTH];
    char *name, *content;
    fpos_t pos;
    char* line;

    line_count = 0;
    //while (fgets(buffer, MAX_LINE_LENGTH, file)) {
    line_count++;
    line = delete_first_spaces(buffer);
    /* Extract the macro name from the declaration line and validate it */

    /* Save the macro content starting from the current file position */
    fgetpos(file, &pos);
    content = save_mcro_content(file, &pos, &line_count);
    if (content == NULL) {
        result = NO;
        //continue;
    }
    else {
        /* going to the end of the macro */
        fsetpos(file, &pos);
        /* adding the new mcro into the mcro_list */
        //add_to_list(head, name, content, mcro_line);
        hashMapInsert(macro_map, macro_name, content);
    }


    // }

    return result;
}


boolean process_macro_file(FILE* file, HashMapPtr macro_map) {
    /* assumes "mcro " has been encountered right before the function was called */
    boolean result = YES;

    char buffer[MAX_LINE_LENGTH];
    char* pos, *tmp;
    int count = 0, line_count = 0;
    const int word_len = strlen(MACRO_START);

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        line_count++;
        pos = delete_first_spaces(buffer);
        pos = strtok(pos, " ");

        if (is_macro_definition(buffer)) {
//            pos = strtok(pos, " ");
            pos = strtok(NULL, " \n");
            if (!valid_mcro_decl(delete_first_spaces(buffer), &pos, line_count))
                result = NO;
            else
                result = result && add_macro_to_map(file, macro_map, pos);

            //process_macro(pos, macro_map);
        }

        pos = buffer;
        while ((pos = strstr(pos, MACRO_START)) != NULL) {
            count++;
            pos += word_len;
        }
    }

    return result;

}

/* Chack macro name
 * validate macro declare
 */

int macro_exec(FILE* file) {
    boolean result = YES;
    rewind(file);

    HashMapPtr macro_map = init_macro_hash_map(file);

    if (!macro_map)
        return NO;
    rewind(file);
    result = process_macro_file(file, macro_map);


    printf("\n%s\n", (char*)hashMapFind(macro_map, "m_macr"));
    return result;
}