
/*About the program: This program simulates an assembler. It gets a list of files names, (using the arguments in command line). Each file contains an assembly language code. It reads and analyzes the code line by line from each file, so at the end of reading proccess, the program prints detailed error messages, if any, or alternatively export files as required: object file (=contains the memory image of the machine code), entries file, externals file, (as detailed later in the program).
Assumptions: *Source files names with '.as' extension. *Each source program provided as input has a possible maximum size. *Excess "white spaces" are ignored in an assembly language input file. *Lowercase and upper case letters are considered different in the assembly language. *Assembly language supports the representation of integers on a decimal base only. *There must be a white character separating (one or more) between command/label and operands, except commas.*/

#include "../include/validators.h"
#include "../include/second_pass.h"
#include "../include/first_pass.h"
#include "../include/utils.h"
#include "../include/first_pass.h"
#include "../include/status_codes.h"
#include "../include/logger.h"
#include "../include/output.h"
#include "../include/preprocess.h"

#define INPUT_FILE_EXTENSION ".as"

/*Array to get line by line*/
char line[MAX_LINE_LENGTH]; 
char* file_name;
FILE* fd;

int DC;
int IC;

int line_counter;
int error_flag;




int iterate_input_files(int argc, char** argv);
boolean process_file(char* asm_file_name);

void reset_assembler();

void prep_second_pass(FILE *file_handle);


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


boolean process_file(char* asm_file_name)
{
    int line_index = 0;
    boolean first_pass_exec_result;

    file_name = (char*)malloc(strlen(asm_file_name) + 4);
    if (file_name == NULL)
        return memoryAllocationFailure;
    /*Opening the file in read permission.*/
    strcpy(file_name, asm_file_name);
    /*Using strcat()- because the user sends the file name without extension*/
    strcat(file_name, INPUT_FILE_EXTENSION);

    fd = open_file(file_name);
    if (fd == NULL) {
        error_log("Could not open file: %s", file_name);
        return openFileError;
    }


    info_log("starting preprocessing on %s", file_name);
    macro_exec(fd);
    rewind(fd);

    info_log("Starting first pass on %s", file_name);
    first_pass_exec_result = first_pass_exec(fd);
    if (first_pass_exec_result != success)
        return first_pass_exec_result;

    prep_second_pass(fd);

    info_log("Starting second pass on %s", file_name);

    first_pass_exec_result = second_pass_exec(fd, &line_index);

    create_output_files(&line_index);

    fclose(fd);


    if (first_pass_exec_result)
        info_log("The file %s has been successfully compiled", file_name);
    else
        info_log("Compilation failed for %s (second pass)", file_name);

    return success;

}


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