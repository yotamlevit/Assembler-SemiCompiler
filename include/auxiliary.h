#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define DC_INITIAL_VALUE 0
#define IC_INITIAL_VALUE 100
#define TABLE_SIZE 150

/*Globals*/
int line_counter, error_flag;
enum flag_state{OFF, ON};
typedef enum { NO, YES } boolean;

/*Structers*/

/*Symbol table definition*/
typedef struct symbols
{
	char symbol_name[MAX_LABEL_LENGTH];
	unsigned address;
	boolean is_attached_directive;
	boolean is_external;
	struct symbols* next;
}symbol;

struct diveded_into_threes
{
	unsigned w1 : 3;
	unsigned w2 : 3;
	unsigned w3 : 3;
	unsigned w4 : 3;
	unsigned w5 : 3;
};

struct data_word_fields
{
	int w : 15;
	unsigned : 0;
	unsigned address;
	union dataword* next;
};

/*Data machine word definition*/
typedef union dataword 
{ 
	struct data_word_fields d;
	struct diveded_into_threes parts;
}data_word;

struct machine_word_fields 
{
	unsigned E : 1;
	unsigned R : 1;
	unsigned A : 1;
	int w : 12;
	unsigned : 0;
	unsigned address;
	union machine_word* next;
};

/*Machine word definition*/
typedef union machine_word
{ 
	struct machine_word_fields c;
	struct diveded_into_threes parts;
}word;

struct code_word_fields
{
	/*fields: external, relocatable, absoloute*/
	unsigned int role : 3;

	/*adrresing mode for desination operand*/
	unsigned destination_immidiate : 1;
	unsigned destination_direct : 1;
	unsigned destination_indirect_register : 1;
	unsigned destination_direct_register : 1;

	/*adrresing mode for source operand*/
	unsigned source_immidiate : 1;
	unsigned source_direct : 1;
	unsigned source_indirect_register : 1;
	unsigned source_direct_register : 1;

	unsigned op_code : 4;

	unsigned : 0;

	unsigned address;
	union machine_word* next;
};

/*Code machine word definition*/
typedef union codeword
{ 
	struct code_word_fields c;
	struct diveded_into_threes parts;
}code_word;

/*Analize input line functions*/
void label_actions(char*);
int is_label(char*);
int isLabel2(char*);

void insert_numerical_data(char*);
void insert_string_data(char*);
void operation(char*);

int is_operation(char*);
int is_stop(char*);
void ext(char*);

/*Analize 2 second pass functions*/
void ent(char*);
void label2(char*);
void second_operation(char* li);

/*Auxiliary functions*/
char* delete_first_spaces(char*);
char* find_comma(char*);
void clean_line(char*);
void clean_label_name(char*);
void add_extension_2_file_name(char*);
void free_code_table();
void free_data_table();
void free_symbol_table();
void free_entries_list();
void free_externals_list();

