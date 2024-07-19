
/*About the program: This program simulates an assembler. It gets a list of files names, (using the arguments in command line). Each file contains an assembly language code. It reads and analyzes the code line by line from each file, so at the end of reading proccess, the program prints detailed error messages, if any, or alternatively export files as required: object file (=contains the memory image of the machine code), entries file, externals file, (as detailed later in the program).
Assumptions: *Source files names with '.as' extension. *Each source program provided as input has a possible maximum size. *Excess "white spaces" are ignored in an assembly language input file. *Lowercase and upper case letters are considered different in the assembly language. *Assembly language supports the representation of integers on a decimal base only. *There must be a white character separating (one or more) between command/label and operands, except commas.*/

#include "../include/auxiliary.h"
#include "../include/validators.h"
#include "../include/second_pass.h"
#include "../include/first_pass.h"
#include "../include/globals.h"
#include "../include/utils.h"

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
int process_file(char* asm_file_name);
int first_pass_exec(FILE* file_handle);
// after first pass
int call_second_pass(FILE* file_handle);

void reset_assembler();


int first_pass_exec(FILE* file_handle)
{
    line_counter = 0;
    error_flag=OFF;
    /*First pass*/
    while (!feof(file_handle))
    {
        /*First analize*/
        analize_input_line(line);
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, file_handle);
    }

    //exe_first_pass(file_name);
    return validate_memory(IC, DC);
}

int call_second_pass(FILE* file_handle)
{
    /*Tmpty the first element of the array*/
    line[0] = '\0';
    /*Return fd to point on the begining of the file.*/
    rewind(file_handle);
    /*Zero the parameters before the next analize*/
    line_counter = 0;
    clean_line(line);
    I = 0;
    /*Update the address of the guide labels in the symbal table*/
    fix_symbol_addresses();

    fd = file_handle;
    /*Second pass*/
    while (!feof(fd))
    {
        /*Second analize*/
        analize_2_second_pass(line);
        line_counter++;
        /*Get one line from the file V */
        fgets(line, MAX_LINE_LENGTH, fd);
    }

    return 1;
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


int process_file(char* asm_file_name)
{
    file_name = (char*)malloc(strlen(asm_file_name) + 4);
    if (!file_name)
    {
        printf("ERROR!! Memory allocation faild\n");
        error_flag = ON;
        exit(0);
    }
    /*Opening the file in read permission.*/
    strcpy(file_name, asm_file_name);
    /*Using strcat()- because the user sends the file name without extension*/
    strcat(file_name, INPUT_FILE_EXTENSION);

    fd = open_file(file_name);

    if (fd == NULL)
        return 0;

    if (!first_pass_exec(fd) || error_flag) // TODO: When refactoring first_pass make first_pass return true of false and not use global error flag
        return 0;

    call_second_pass(fd);

    fclose(fd);

    if (!validate_second_pass())
        return 0;

    printf("The file %s has been successfully compiled\n", file_name);

    reset_assembler();

    return 1;
}


int iterate_input_files(int argc, char** argv)
{
    char* asm_file_name;
    int i;
    for (i = 1; i < argc; i++)
    {
        asm_file_name = argv[i];

        printf("\nStart processing file: %s\n", asm_file_name);

        process_file(asm_file_name);
        //if (!process_file(asm_file_name))
          //  return 0;
    }

    return 1;
}


int main(int argc, char** argv)
{

    if (!validate_input(argc, argv))
        return 0;

    reset_assembler();

    if (iterate_input_files(argc, argv))
        return 1;

    return 0;
}