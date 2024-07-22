/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_SECOND_PASS_H
#define ASSEMBLER_SEMICOMPILER_SECOND_PASS_H

#include <stdio.h>
#include "auxiliary.h"


boolean analize_2_second_pass(char*);

int second_pass_exec(FILE * file_handle);

int isLabel2(char*);

/*Analize 2 second pass functions*/
void process_entry(char*);

void process_lable(char*);

boolean second_operation(char* li);


void handle_error(const char* message);

int find_symbol(const char* li, int length);

#endif
