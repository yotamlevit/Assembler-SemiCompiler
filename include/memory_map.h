/*
 * Created by Yotam Levit on 18/07/2024.
*/

#ifndef ASSEMBLER_SEMICOMPILER_MEMORY_MAP_H
#define ASSEMBLER_SEMICOMPILER_MEMORY_MAP_H

/*Output functions*/
void create_object_file();
void create_entry_file();
void create_external_file();
void create_output_files(int* line_counter);

#endif
