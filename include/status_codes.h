/*
 * Created by Hadar Nir on 20/07/2024.
 */

#ifndef ASSEMBLER_SEMICOMPILER_STATUS_CODES_H
#define ASSEMBLER_SEMICOMPILER_STATUS_CODES_H


#define MAX_STATUS_LEN 1000

typedef enum {FALSE, TRUE} boolean;

typedef enum {
    success,
    genericError,
    undefinedSetNameError,
    undefinedCommandNameError,
    valueError,
    missingEndOfListError,
    typeError,
    missingParamError,
    missingCommand,
    troubleParsingCommandArguments,
    troubleParsingCommandSETS,
    tooManySets,
    extraTextAfterCommand,
    troubleInitCommands,
    troubleInitSet,
    troubleInitUserBuffer,
    illegalComma,
    consecutiveCommas,
    missingComma,
    NumberOfErrors
} StatusCode;

typedef struct {
    int status_id; /*The ID of the status.*/
    char status_msg[MAX_STATUS_LEN]; /*The status message associated with the status.*/
} Status, * StatusPtr;


StatusPtr getStatus(StatusCode statusCode);
void printStatus(StatusCode statusCode);

#endif /*ASSEMBLER_SEMICOMPILER_STATUS_CODES_H*/
