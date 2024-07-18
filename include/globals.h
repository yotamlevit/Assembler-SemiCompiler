/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_GLOBALS_H
#define ASSEMBLER_SEMICOMPILER_GLOBALS_H


/*Tables and lists*/
extern symbol* head_symbol;
extern symbol* head_entries;
extern symbol* head_externals;
extern code_word code_table[TABLE_SIZE];
extern data_word* data_table[TABLE_SIZE];

//extern char line[MAX_LINE_LENGTH];


/*Index*/
extern int I;
extern int D;
/*index of code_table, index of data_table, instruction counter, data counter.*/
extern int IC;
/*index of code_table, index of data_table, instruction counter, data counter.*/
extern int DC;
#endif

