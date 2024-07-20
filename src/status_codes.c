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
        {success, "The operation was successful"},
        {genericError, "Unknown Error occurred"},
        {noFilesProvided, "ERROR!! You must send files."},
        {undefinedSetNameError, "Undefined set name"},
        {undefinedCommandNameError, "Undefined command name"},
        {valueError, "Invalid set member – value out of range"},
        {missingEndOfListError, "List of set members is not terminated correctly"},
        {typeError, "Invalid set member – not an integer"},
        {missingParamError, "Missing parameter"},
        {missingCommand, "Missing command"},
        {troubleParsingCommandArguments, "There was a problem parsing command arguments"},
        {troubleParsingCommandSETS, "There was a problem parsing command sets"},
        {tooManySets, "Too many sets in command"},
        {extraTextAfterCommand, "Extraneous text after end of command"},
        {troubleInitCommands, "There was a problem initializing a command"},
        {troubleInitSet, "There was a problem initializing a set"},
        {troubleInitUserBuffer, "There was a problem initializing a user buffer"},
        {illegalComma, "Illegal comma"},
        {consecutiveCommas, "Multiple consecutive commas"},
        {missingComma, "Missing comma"}
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