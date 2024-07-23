//
// Created by Yotam Levit on 18/07/2024.
//

#include "../include/validators.h"
#include "../include/auxiliary.h"
#include "../include/memory_map.h"
#include "../include/globals.h"
#include "../include/status_codes.h"

int validate_input(int argc, char** argv)
{
    if (argc == 1)
        return noFilesProvided;
    return success;
}

int validate_memory(int IC, int DC)
{
    /*Checking memory limit exceeded.*/
    if (IC + DC > MEMORY_SIZE)
    {
        printf("ERROR!! The program has exceeded the memory limits.\n");
        return 0;
    }

    return 1;
}

void create_output_files(int* line_counter)
{
    I = *line_counter;
    create_object_file();
    create_entry_file();
    create_external_file();
}
