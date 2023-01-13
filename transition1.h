struct node {
	unsigned int address: 10;/*address of the symbol in the memory*/
	unsigned int type:2;/*0 is absolute, 1 is external, 2 is reloctable*/
	unsigned int data:1;/*data is 0, command is 1*/
	char name[LINESIZE/2];
	struct node *next;

};/*for labels*/
struct node1 {
	
	unsigned int code: 10;
	unsigned int type: 1;	/*data is 0, command is 1*/
	struct node1 *next;
};/*for commands*/
int start_transition();/*Starts the first transition*/
int transition2();/*Starts the second transition*/

void set_next(int y, int datatype, char *name, int data_type);/*Sets next label*/
void set_next1(int y, int type_);/*Sets next command*/
int free_nodes();/*frees the label linked list*/

int build_op_code(char *line);/*builds the first line of opcode for a command*/
int free_nodes1();/*frees the computer commands linked list*/

int check_number(char*);/*checks if a specific string is a number, 
returns 1 if it is, 0 otherwise*/

int is_extern_or_entry(char* line);/*checks if the line of code in assembly is a reference to extern or entry, returns 1 if extern, 2 if entry 0 if not one of them and -1 if theres a syntax error*/

char* clear_spaces(char* x);/*clears the spaces from the start of a string*/
int adressing(char* x);/*returns the שיטת מיעון of the specific command*/

int mark_entry(char* line);/*marks a certain label as an entry label*/
int how_many_arguments(char* line);/*returns how many arguments does a line have, seperated by a comma or space*/

int finalize_op_code(char *line, int code);/*creates the 2nd and above lines of computer code for a command*/

int create_files();/*creates the .ext .ent and the .ob files for the assembly file*/
int second;/*checks if we're already in the second transition or not*/

short L; /*The number of computer lines for a single line of code*/
short ERROR;/*Error code*/
struct node *head;
struct node *current;
struct node1 *code_head;
struct node1 *code_current;
short size_of_comms;/*size of commands linked list*/
short size_of_data;/*size of label linked list*/
short DC, IC;
