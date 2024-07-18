//
// Created by Yotam Levit on 18/07/2024.
//

#include "../include/validators.h"
#include "../include/assembler.h"
#include "../include/auxiliary.h"
#include "validators.h"

int validate_input(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("ERROR!! You must send files.\n");
        return 0;
    }
    return 1;
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

int validate_second_pass()
{
    if (!error_flag)
    {
        create_object_file();
        create_entry_file();
        create_external_file();

        return 1;
    }

    return 0;
}