
#ifndef ASSEMBLER_SEMICOMPILER_OUTPUT_H
#define ASSEMBLER_SEMICOMPILER_OUTPUT_H

/*Output functions*/
void create_object_file(char* file_name);
void create_entry_file(char* file_name);
void create_external_file(char* file_name);
void create_output_files(char* file_name, int* line_index);

#endif
