
/*About the program: This program simulates an assembler. It gets a list of files names, (using the arguments in command line). Each file contains an assembly language code. It reads and analyzes the code line by line from each file, so at the end of reading proccess, the program prints detailed error messages, if any, or alternatively export files as required: object file (=contains the memory image of the machine code), entries file, externals file, (as detailed later in the program).
Assumptions: *Source files names with '.as' extension. *Each source program provided as input has a possible maximum size. *Excess "white spaces" are ignored in an assembly language input file. *Lowercase and upper case letters are considered different in the assembly language. *Assembly language supports the representation of integers on a decimal base only. *There must be a white character separating (one or more) between command/label and operands, except commas.*/

#include "../include/validators.h"
#include "../include/second_pass.h"
#include "../include/first_pass.h"
#include "../include/utils.h"
#include "../include/status_codes.h"
#include "../include/logger.h"
#include "../include/output.h"
#include "../include/preprocess.h"

#define INPUT_FILE_EXTENSION ".as"
#define FILE_READ "r"

/*Array to get line by line*/
char line[MAX_LINE_LENGTH]; 
char* file_name;
FILE* fd;

int DC;
int IC;

int line_counter;
int error_flag;

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
    rewind(file_handle); // TODO move rewind to each place that needed
    /*Zero the parameters before the next analize*/
    line_counter = 0;
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
    line_counter = 0;
    /*Clean & Free all tables.*/
    free(file_name);
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
    int line_index = 0;
    boolean result;

    file_name = (char*)malloc(strlen(asm_file_name) + 4);
    if (file_name == NULL)
        return memoryAllocationFailure;
    /*Opening the file in read permission.*/
    strcpy(file_name, asm_file_name);
    /*Using strcat()- because the user sends the file name without extension*/
    strcat(file_name, INPUT_FILE_EXTENSION);

    fd = open_file(file_name, FILE_READ);
    if (fd == NULL)
        return openFileError;

    info_log("Starting preprocessing on %s", file_name);
    result = macro_exec(fd, file_name); /* TODO: Convert return type to StatusCode */
    fclose(fd);

    if (result == NO)
        return failedPreprocess;

    fd = open_file(file_name, FILE_READ);
    if (fd == NULL)
        return openFileError;

    info_log("Starting first pass on %s", file_name);
    result = first_pass_exec(fd);
    if (result != success){
        printStatus(result);
        fclose(fd);
        return failedFirstPass;
    }
    info_log("Finished first pass sucessfully on %s", file_name);

    prep_second_pass(fd); /* TODO: Return StatusCode and add logs of the operation */
    rewind(fd);

    info_log("Starting second pass on %s", file_name);

    result = second_pass_exec(fd, &line_index); /* TODO: Convert return type to StatusCode */

    create_output_files(&line_index); /* TODO: Return StatusCode and add logs of the operation */

    fclose(fd);

    if (result)
    {
        info_log("The file %s has been successfully compiled", file_name);
        return success;
    }
    else{
        info_log("Compilation failed for %s (first pass)", file_name);
        return failedSecondPass;
    }
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
            printStatus(process_file_result);
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
    int validate_input_result = validate_input(argc, argv);
    if (validate_input_result != success) {
        printStatus(validate_input_result);
        return error;
    }

    int iterate_input_files_result = iterate_input_files(argc, argv);
    if (iterate_input_files_result != success)
        return error;

    return success;
}