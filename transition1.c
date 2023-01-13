#include "data.h"
#include "transition1.h"

void set_next(int y, int datatype, char* name, int is_data) {
	struct node *ptr=malloc(sizeof(struct node));
	
	if(ptr==NULL) {
		fprintf(stderr,"Memory allocation failed\n");
		exit(1);
	}
	ptr->type=datatype;
	ptr->address=y;
	strcpy(ptr->name,name);
	ptr->next=NULL;
	ptr->data=is_data;
	current->next=ptr;

	current=ptr;
	if (size_of_data==0) {/*if its the first node*/
		head=current;
	}
	
	size_of_data++;
}
void set_next1(int y, int type_) {
	
	struct node1 *ptr=malloc(sizeof(struct node));
	
	if(ptr==NULL) {
		fprintf(stderr,"Memory allocation failed\n");
		exit(1);
	}
	
	ptr->code=y;
	ptr->type=type_;
	ptr->next=NULL;
	
	code_current->next=ptr;
	code_current=ptr;
	
	if (size_of_comms==0) {/*if its the first node*/
		code_head=code_current;
	}
	
	size_of_comms++;
	
}
int free_nodes() {
	current=head;
	while (head!=NULL) {
		head=head->next;
		free(current);
		current=head;
	}
	return 0;
}
int free_nodes1() {
	code_current=code_head;
	while (code_head!=NULL) {
		code_head=code_head->next;
		free(code_current);
		code_current=code_head;
	}
	return 0;
}

int start_transition() {
	
	FILE *fp;
	int code;
	int line_number;
	int label_on;
	int storage_type;
	char label_name[LINESIZE];
	char line[LINESIZE];
	char line_copy[LINESIZE];
	char *after_label;/*code after the label name*/
	label_on=0;
	second=0;
	IC=0, DC=0;
	size_of_data=0;
	size_of_comms=0;
	line_number=0;
	head=malloc(sizeof(struct node));
	code_head=malloc(sizeof(struct node1));
	
	current=head;
	code_current=code_head;
	fp=fopen(newfilename,"r");
	

	

	if (fp==NULL) {
		fprintf(stderr,"File does not exist!");
		return -2;	
	}
	
	while(fgets(line,LINESIZE,fp)!=NULL) {
		if (line[strlen(line)-1]!=10) {
			line[strlen(line)]='\0';
		}else{
			line[strlen(line)-1]='\0';
		}
		if (ERROR>0) {/*resets the error code so we know an error has occured, but dont print the same error message again*/
			
			ERROR=-999;
		}
		label_on=0;
		L=0;
		line_number++;
	
		strcpy(line_copy,line);
		/*to check if line is blank or is a comment*/
		if (is_empty_line(line_copy)==1) {
			continue;
		}
		/*checks if the line is a label declaration and gets label name if so*/
		after_label=line_copy;
		after_label=is_label(after_label);
		
		if (after_label!=NULL) {/*turns on flag of label_on*/
			label_on=1;
			strcpy(label_name,after_label);
		}	
		/*if a label already exists, print an error message*/
		if (label_on == 1 && label_exists(label_name)==1) {
			ERROR=3;
			print_error_message(line_number,line);
		}
		strcpy(line_copy,line);
		/*sets the string after_label to the code that comes after the label, so we use that instead of the entire line, if there isnt a label it'll just set it to the original line*/
		if (label_on==1) {
			
			after_label=strchr(line_copy,':')+1;
			after_label=clear_spaces(after_label);
			
			
		}else{
			after_label=line_copy;
		}
		
		storage_type=is_storage(after_label);
		
		if (storage_type<0) {
			/*error*/	
			print_error_message(line_number, line);
			continue;
		
		}else if(storage_type>0) {
			/*handles the .data, .string or .struct situations*/
			if (label_on==1) {
				set_next(DC,2,label_name,1);
				/*sets the next label with DC, relocatable type and 					marks it as data.*/
			}
			DC+=L;
			print_error_message(line_number, line);
			continue;
		}
		
		storage_type=is_extern_or_entry(after_label);
		/*storage_type is reused, it functions as the value of 			is_extern_or_entry at the moment*/
		
		if (storage_type!=0) {	
			/*if its an extern or entry, there is no need to continue
			the functions take care of extern*/
			print_error_message(line_number, line);
			continue;
		}
		
		if (label_on==1) {
			if (label_exists(label_name)==1) {
				/*if a label is overriden, give an error message*/
				ERROR=3;
				print_error_message(line_number, line);
				continue;
			}
			/*sets the label with IC, relocatable and command type*/
			set_next(IC,2,label_name,0);
		}
		/*builds opcode for the first command*/
		code= build_op_code(after_label);
		
		if (code<0) {
			/*error*/
			print_error_message(line_number, line);
			continue;
		}else{
			IC+=L;
			set_next1(code,1);
			L--;
			
			while (L!=0) { /*creates the amount of code lines needed*/
				set_next1(0,1);
				L--;
			}
		}
		
		print_error_message(line_number, line);
			
	}
	fclose(fp);
	IC+=100;/*adds 100 to ic since addresses start at 100*/
	if(ERROR!=0) {
		free_nodes();/*frees linked lists for the next file in case of an 					error*/
		free_nodes1();
		return -1;
	}
	current=head;
	
	while (current) {
		if ((current->type)!=1) {/*adds final values*/
			
			if ((current->data)==1) {
				current->address=current->address+IC;
			}else {
				current->address=current->address+100;
			}
		}
		
		current=current->next;
	
	}
	
	
	transition2();/*intiates 2nd transition*/
	
	return 0;
}
