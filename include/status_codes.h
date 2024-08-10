/*
 * Created by Hadar Nir on 20/07/2024.
 */

#ifndef ASSEMBLER_SEMICOMPILER_STATUS_CODES_H
#define ASSEMBLER_SEMICOMPILER_STATUS_CODES_H


#define MAX_STATUS_LEN 1000

// typedef enum {FALSE, TRUE} boolean;

typedef enum {
    success,
    error,
    noFilesProvided,
    memoryAllocationFailure,
    openFileError,
    failedPreprocess,
    failedFirstPass,
    failedSecondPass,
    NumberOfErrors
} StatusCode;

typedef struct {
    int status_id; /*The ID of the status.*/
    char status_msg[MAX_STATUS_LEN]; /*The status message associated with the status.*/
} Status, * StatusPtr;


StatusPtr get_status(StatusCode statusCode);
char* get_status_message(StatusCode statusCode);

#endif /*ASSEMBLER_SEMICOMPILER_STATUS_CODES_H*/
