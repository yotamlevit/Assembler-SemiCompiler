#define INPUT_FILE_EXTENSION ".as"
#define MEMORY_SIZE 4096

void handle_input(int, char**);

/*Output functions*/
void create_object_file();
void create_entry_file();
void create_external_file();

/*Two transition functions*/
void analize_input_line(char*);
void analize_2_second_pass(char*);

void fix_symbol_addresses();

/*Tables and lists*/
extern symbol* head_symbol;
extern symbol* head_entries;
extern symbol* head_externals;
extern code_word code_table[TABLE_SIZE];
extern data_word* data_table[TABLE_SIZE];

/*Index*/
extern int I;
extern int D;
extern int IC;
extern int DC;
