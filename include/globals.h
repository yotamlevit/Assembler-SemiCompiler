/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_GLOBALS_H
#define ASSEMBLER_SEMICOMPILER_GLOBALS_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define DC_INITIAL_VALUE 0
#define IC_INITIAL_VALUE 100

extern char line[MAX_LINE_LENGTH];


/*Index*/
extern int I;
extern int D;
/*index of code_table, index of data_table, instruction counter, data counter.*/
extern int IC;
/*index of code_table, index of data_table, instruction counter, data counter.*/
extern int DC;
/*Globals*/
extern int line_counter;
/*Globals*/
extern int error_flag;
#endif

