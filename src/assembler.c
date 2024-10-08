#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/validators.h"
#include "../include/second_pass.h"
#include "../include/first_pass.h"
#include "../include/utils.h"
#include "../include/status_codes.h"
#include "../include/logger.h"
#include "../include/output.h"
#include "../include/preprocess.h"
#include "../include/hash_map.h"

#define INPUT_FILE_EXTENSION ".as"
#define FILE_READ "r"

/*Array to get line by line*/
char line[MAX_LINE_LENGTH];
FILE* fd;

int DC;
int IC;

/**
 * @brief Prepares the assembler for the second pass.
 *
 * The prep_second_pass function resets the necessary state and prepares the assembler
 * for the second pass of file processing. It empties the line buffer, resets the file
 * pointer to the beginning, zeros the line counter, cleans the line buffer, resets the
 * code table index, and updates the addresses of guide labels in the symbol table.
 *
 * @param file_handle A pointer to the file being processed.
 * @return void This function does not return a value.
 */
void prep_second_pass(FILE *file_handle) {/*Tmpty the first element of the array*/
    line[0] = '\0';
    /*Return fd to point on the begining of the file.*/
    rewind(file_handle);
    /*Zero the parameters before the next analize*/
    clean_line(line);
    I = 0;
    /*Update the address of the guide labels in the symbal table*/
    fix_symbol_addresses();
}


/**
 * @brief Resets the assembler's state for a new file processing.
 *
 * The reset_assembler function initializes the assembler's data and instruction counters,
 * zeroes the data and code table indices, and resets the line counter. It also frees
 * any allocated memory for file names and clears all relevant tables and lists.
 *
 * @return void This function does not return a value.
 */
void reset_assembler()
{
    /*Initialization: zero the data table i, code table i, data counter, instruction counter*/
    D = 0;
    I = 0;
    DC = DC_INITIAL_VALUE;
    IC = IC_INITIAL_VALUE;
    /*Clean & Free all tables.*/
    free_data_table();
    free_code_table();
    free_symbol_table();
    free_entries_list();
    free_externals_list();
}


/**
 * @brief Processes the specified assembly file through multiple passes.
 *
 * The process_file function performs preprocessing, first pass, and second pass on the
 * specified assembly file. It opens the file, executes each pass, and handles errors.
 * If any stage fails, it returns the corresponding error status.
 *
 * @param asm_file_name The name of the assembly file to be processed.
 * @return A StatusCode representing the result of the file processing.
 *         Returns success if all stages complete successfully. Otherwise, returns
 *         an appropriate error code indicating the failure stage.
 */
StatusCode process_file(char* asm_file_name)
{
    char* file_name;
    int line_index = 0;
    boolean result;
    HashMapPtr macro_map = NULL;
    FILE* as_file;

    file_name = (char*)malloc(strlen(asm_file_name) + 4);
    if (file_name == NULL)
        return memoryAllocationFailure;
    /*Opening the file in read permission.*/
    strcpy(file_name, asm_file_name);
    /*Using strcat()- because the user sends the file name without extension*/
    strcat(file_name, INPUT_FILE_EXTENSION);

    as_file = open_file(file_name, FILE_READ);
    if (as_file == NULL)
        return openFileError;

    info_log("Starting preprocessing on %s", file_name);
    result = macro_exec(as_file, file_name, &macro_map);

    fclose(as_file);

    if (result == FALSE)
        return failedPreprocess;
    info_log("Finished preprocessing successfully");


    fd = open_file(file_name, FILE_READ);
    if (fd == NULL)
        return openFileError;

    info_log("Starting first pass on %s", file_name);
    result = first_pass_exec(fd, macro_map, &line_index);
    hashMapFree(macro_map);
    if (!result){
        fclose(fd);
        return failedFirstPass;
    }
    info_log("Finished first pass successfully on %s", file_name);
    line_index = 0;

    prep_second_pass(fd);
    rewind(fd);

    info_log("Starting second pass on %s", file_name);

    result = second_pass_exec(fd, &line_index);

    create_output_files(file_name, &line_index);

    fclose(fd);

    if (!result)
        return failedSecondPass;

    info_log("The file %s has been successfully compiled", file_name);
    return success;
}


/**
 * @brief Iterates through input files and processes each one.
 *
 * The iterate_input_files function processes each input file provided as a command-line
 * argument. It resets the assembler for each file, logs the start of processing,
 * and processes the file. If any file fails to process, it prints the status and
 * sets the return status to error.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of pointers to the command-line arguments.
 * @return An integer representing the status of the file processing.
 *         Returns success if all files are processed successfully, otherwise returns error.
 */
int iterate_input_files(int argc, char** argv)
{
    int status = success, process_file_result;
    char* asm_file_name;
    int i;
    for (i = 1; i < argc; i++)
    {
        reset_assembler();
        asm_file_name = argv[i];
        info_log("Start processing file: %s", asm_file_name);
        process_file_result = process_file(asm_file_name);
        if (process_file_result != success) {
            error_log(get_status_message(process_file_result));
            status = error;
        }
    }
    return status;
}


/**
 * @brief Main entry point of the program.
 *
 * The main function validates the input arguments and processes the input files.
 * If the input validation fails, it prints the status and returns an error code.
 * Otherwise, it processes the input files and returns a success code if no errors occur.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of pointers to the command-line arguments.
 * @return An integer representing the exit status of the program.
 *         Returns success if all operations succeed, otherwise returns error.
 */
int main(int argc, char** argv)
{
    int iterate_input_files_result;
    int validate_input_result = validate_input(argc);
    if (validate_input_result != success) {
        error_log(get_status_message(validate_input_result));
        return error;
    }

    iterate_input_files_result = iterate_input_files(argc, argv);
    if (iterate_input_files_result != success)
        return error;

    info_log(get_status_message(success));
    return success;
}
