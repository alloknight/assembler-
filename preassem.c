#include "data.h"
#include "transition1.h"


char saved_words[RESERVEDWORDS][6] ={"r0","r1","r2","r3","r4","r5","r6","r7"
			,"mov","cmp","add","sub","not","clr","lea"
			,"inc","dec","jmp","bne","get","prn","jsr"
			,"rts","hlt"};/*words reserved for assembly*/
unsigned hash (char *s)
{
 	unsigned hashval;
 	for (hashval = 0; *s != '\0'; s++)
 		hashval = *s + 31 * hashval;
 	return hashval % HASHSIZE;
}
struct nlist *lookup (char *s)
{

 	struct nlist *np;
		
 		for (np=hashtab[hash(s)]; np != NULL; np = np -> next) {
 			if (strcmp (s, np->name) == 0) {
 				return np;
			}
		}
	 
	 return NULL;
}

struct nlist *install(char *name, char *defn)
{
 	struct nlist *np;
 	unsigned int hashval;
	
 	if ((np = lookup (name)) == NULL) {
		
 		np = malloc (sizeof(*np));
		
		if (np == NULL) {
 			printf ("malloc: failed in install\n");
 			exit (2);
		}
		np->name = malloc(sizeof(char)*strlen(name));
		strcpy(np->name,name);

		if (np->name==NULL) {
			printf ("malloc: failed in install\n");
 			exit (2);
		}
		hashval = hash(name);
 		np->next = hashtab[hashval]; 
	}
	else 
 		free((void *) np->defn);
 	/* Allocated by strdup */
	np->defn=malloc(strlen(defn)*sizeof(char));
	strcpy(np->defn,defn);
 	if (np->defn == NULL)
	{
 	printf("install: failed on strdup\n");
 	exit (3);
	}
	return np;
}

/*----------------*/
int macroOpener(char *filename) {
	FILE *fp;
	FILE *newfile;
	int i;
	int macro_flag=0;
	char *macro_name;
	char macro_content[LINESIZE*5]; /*macro content is blocked to five full lines*/
	char line[LINESIZE];
	char line_copy[LINESIZE];
	char current_macro_name[LINESIZE];
	struct nlist *macro;
	macro_content[0]='\0';
	strcpy(line_copy,filename);
	fp= fopen(strcat(line_copy,TOKEN),"r");/*opens .as file name*/
	for (i = 0; i< HASHSIZE; i++) {/*frees elements of previous files*/
		if (hashtab[i]!=NULL) {
			free(hashtab[i]);
		}
 		hashtab[i] = NULL; 
	}
	if (fp==NULL) {
		fprintf(stderr,"Invalid file name %s\n",filename);
		return -1;
	}
	/*Creats a new file name, with the old name without the .as, and the new name 		with .am*/

	newfilename=strcat(filename,TOKENOPENED);/*.am file name*/
	remove(newfilename);/*removes the old file if it exists*/
	
	newfile=fopen(newfilename,PERMS);

	while(fgets(line,LINESIZE,fp)!=NULL) {
		for (i=0; i<strlen(line); i++) {
			if (line[i]=='\t') {/*replaces tabs with spaces*/
				line[i]=' ';
			}
		}
		strcpy(line_copy,line);
		/*handles macro behavior*/
		if (macro_flag==1) {
			char *x;
			x=strtok(line_copy," ");
			x[strlen(x)-1]='\0';
			if (strcmp(x,"endmacro")==0) {
				/*copies all the content written between macro declaration and endmacro into the macro content in the hash-table*/
				macro_flag=0;
				hashtab[hash(current_macro_name)]=install(current_macro_name,macro_content);
				strcpy(macro_content,"");
			}else{
				/*copies current line into the macro*/
				strcat(macro_content,line);
			}
			continue;
		}
		/*copy macro content when requested*/
		if ((macro_name=get_macro(line_copy))!=NULL && (macro=lookup(macro_name))!=NULL) {
			fprintf(newfile,"%s",macro->defn);
			continue;
		}
		strcpy(line_copy,line);
		/*find macro declaration*/
		if((macro_name=is_macro_decleration(line_copy))!=NULL) {
			strcpy(current_macro_name,macro_name);
			macro_flag=1;
		}else {
			
			fprintf(newfile,"%s",line);/*copies current line*/
		}

	}
	fclose(fp);
	fclose(newfile);
	
	return 0;
	
}
char* replace_tab_with_space(char *s) {
	int i;
	for (i=0; i<strlen(s); i++) {
		if (s[i]=='\t') {
			s[i]=' ';
		}
	}
	return s;
}

char* is_macro_decleration(char *s) {
	
	int i;
	char *tmp;
	s=replace_tab_with_space(s);
	if (strcmp(strtok(s," "),"macro")==0) {
		
		tmp=strtok(NULL," ");/*gets the macro name without spaces*/
		
		for (i=0; i<RESERVEDWORDS; i++) {
			if (strcmp(tmp,saved_words[i])==0) {/*saved name*/
				fprintf(stderr,"Macro name %s cannot be used\n",tmp);
				return NULL;
			}
			if (strtok(NULL," ")!=NULL) {/*too many arguments*/
				fprintf(stderr, "Too many arguments!\n");
				return NULL;
			}

			
		}
		
		strcpy(s,tmp);/*returns macro name*/
		return s;
		
	}

	return NULL;
}
char* get_macro(char *s) {
	
	char *s1;
	s=replace_tab_with_space(s);                
	s=strtok(s," ");
	s1=strtok(NULL," ");
	if (s1==NULL) {
		return s;
	}
	return NULL;
}


int main(int argc, char** argv) {
	int i;
	char s[LINESIZE];

	for (i = 0; i< HASHSIZE; i++)/*clears the hashtable*/
 		hashtab[i] = NULL; 
	for (i=1; i<argc; i++) {/*goes over all the files*/
		strcpy(s,argv[i]);
		ERROR=0;
		printf("===========%s===========\n",argv[i]);
		
		if (macroOpener(s)==-1) {
			printf("File %s failed to assemble!\n",argv[i]);
			continue;
		}
		
		if (start_transition()==-2) {
			printf("File %s failed to assemble!\n",argv[i]);
			continue;
		}
		
		if (ERROR==0) {
			printf("File %s successfully assembled!\n",argv[i]);
		}else{
			printf("File %s failed to assemble!\n",argv[i]);
		}
		
		
	}

	
	
	return 0;
}


