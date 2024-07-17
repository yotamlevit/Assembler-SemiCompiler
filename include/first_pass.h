#define MAX_STRING 75
#define OPERATIONS_AMOUNT 16

/*Tables and lists*/
extern symbol* head_symbol;
extern code_word code_table[150];
extern data_word* data_table[150];

extern symbol* head_entries;
extern symbol* head_externals;

extern char line[MAX_LINE_LENGTH];

/*Index*/
extern int IC;
extern int DC;

extern int I;
extern int D;

/*Functions*/
char addressing_mode(char*);

/*Two transition functions*/
void analize_input_line(char*);
void analize_2_second_pass(char*);
