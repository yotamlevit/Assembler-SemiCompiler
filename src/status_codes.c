/*
 * Created by Hadar Nir on 20/07/2024.
 */

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
StatusPtr get_statuses(void)
{
    static Status Statuses[NumberOfErrors] = {
        {success, "The operation was successful"},
        {error, "Error occurred"},
        {noFilesProvided, "You must send files."},
        {memoryAllocationFailure, "Memory allocation failure"},
        {openFileError, "Could not open a file"},
        {failedPreprocess, "Could not finish preprocessing"},
        {failedFirstPass, "Could not finish first pass"},
        {failedSecondPass, "Could not finish second pass"},
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
StatusPtr get_status(StatusCode statusCode)
{
    StatusPtr statuses;
    statuses = get_statuses();
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
char* get_status_message(StatusCode statusCode)
{
    return get_status(statusCode)->status_msg;
}