#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define LINESIZE 81
#define HASHSIZE 101
#define TOKEN ".as"
#define TOKENOPENED ".am"
#define PERMS "w+"
#define RESERVEDWORDS 25
char *newfilename;/*.am file name*/

/*A linked list made to store each macro.*/

struct nlist
{
 	char *name; /* defined name */
 	char *defn; /* replacement text */
 	struct nlist *next; /* next entry in chain */
};
struct nlist *hashtab[HASHSIZE];/*hash table*/
unsigned hash (char *s); /*hashes a string into a key value*/
struct nlist *install(char *name, char *defn); /*install a name into the macro list*/
struct nlist *lookup (char *s);/*looks up if a name exists*/
int macroOpener(char*);/*Opens up macros*/
char* findFileName(char* s);/*gets the file name from a .as format*/
char* get_macro(char *s); /*Gets the macro name without spaces/tabs*/
char* is_macro_decleration(char *s);/*checks if a line is a decleration of a macro*/


int is_saved_word(char*);/*Checks if a certain word is saved*/
int is_storage(char*);/*Checks if a line stores something*/
/*if its a struct, return 1; if string return 2; if data return 3; if not return 0;*/

char* is_label(char* line);/*Checks if a line refers to a label*/
char convert(int num);/*converts a number between 0 and 31 to custom base 32*/
int name_to_opcode(char *s);/*converts command name to code, for example mov->0*/
char* get_num_coverted(int num, char* s); /*converts a number between 0 and 1024 to
 the custom base*/
void print_error_message(int line_num, char* line);/*prints an error message*/
int label_exists(char* word);/*checks if a label exists*/
int is_empty_line(char* line);/*checks if a line is empty or is a comment*/

