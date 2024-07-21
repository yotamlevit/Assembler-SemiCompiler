/*
 * Created by Hadar Nir on 20/07/2024.
 */

#include <stdio.h>
#include "../include/status_codes.h"

/**
 * @brief Retrieves a static array of status messages.
 *
 * This function returns a pointer to a static array of `Status` structures,
 * each representing a predefined status message. The array includes
 * various status messages corresponding to different status codes.
 *
 * @return StatusPtr A pointer to the static array of `Status` structures.
 */
StatusPtr getStatuses(void)
{
    static Status Statuses[NumberOfErrors] = {
        {success, "INFO - The operation was successful"},
        {error, "ERROR - Error occurred"},
        {noFilesProvided, "ERROR - You must send files."},
        {memoryAllocationFailure, "ERROR - Memory allocation failure"},
        {openFileError, "ERROR - Could not open a file"}
    };
    return Statuses;
}

/**
 * @brief Retrieves a status message pointer based on the given status code.
 *
 * This function fetches a list of status messages and returns a pointer to the
 * status message corresponding to the given status code. It is assumed that
 * the status code starts from 2, hence the index is adjusted by subtracting 2.
 *
 * @param statusCode The status code for which to retrieve the status message.
 * @return StatusPtr A pointer to the status message corresponding to the status code.
 */
StatusPtr getStatus(StatusCode statusCode)
{
    StatusPtr statuses;
    statuses = getStatuses();
    return &statuses[statusCode];
}

/**
 * @brief Prints the status message corresponding to the given status code.
 *
 * This function retrieves the status message for the provided status code and prints it
 * to the standard output.
 *
 * @param statusCode The status code for which to print the status message.
 */
void printStatus(StatusCode statusCode)
{
    printf("%s\n", getStatus(statusCode)->status_msg);
}