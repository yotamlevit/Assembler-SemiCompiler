
#include "../include/validators.h"
#include "../include/logger.h"
#include "../include/status_codes.h"


/**
 * Validates the input arguments for the program.
 *
 * @param argc The number of arguments provided.
 * @return An integer status code indicating success or failure:
 *         - noFilesProvided (1) if no files were provided.
 *         - success (0) if the input is valid.
 */
int validate_input(int argc)
{
    /* Check if no files were provided as input */
    if (argc == 1)
        return noFilesProvided;
    return success;
}


int validate_memory(int IC, int DC)
{
    /*Checking memory limit exceeded.*/
    if (IC + DC > MEMORY_SIZE)
    {
        error_log("The program has exceeded the memory limits.\n");
        return 0;
    }

    return 1;
}

