#include "data.h"
#include "transition1.h"
#define BASE 32

extern char saved_words[RESERVEDWORDS][6];
char* get_num_coverted(int num, char* s) {
	char temp[3];
	temp[0]=convert(num/BASE);
	temp[1]=convert(num%BASE);
	temp[2]='\0';
	strcpy(s,temp);
	return s;
}
char convert(int num) {
	switch (num) {
		case 0: return '!';
		case 1: return '@';
		case 2: return '#';
		case 3: return '$';
		case 4: return '%';
		case 5: return '^';
		case 6: return '&';
		case 7: return '*';
		case 8: return '<';
		case 9: return '>';
		case 10: return 'a';
		case 11: return 'b';
		case 12: return 'c';
		case 13: return 'd';
		case 14: return 'e';
		case 15: return 'f';
		case 16: return 'g';
		case 17: return 'h';
		case 18: return 'i';
		case 19: return 'j';
		case 20: return 'k';
		case 21: return 'l';
		case 22: return 'm';
		case 23: return 'n';
		case 24: return 'o';
		case 25: return 'p';
		case 26: return 'q';
		case 27: return 'r';
		case 28: return 's';
		case 29: return 't';
		case 30: return 'u';
		case 31: return 'v';
	}
	return -1;
}
char* clear_spaces(char* x) {
	while(x[0]==' ') {
		x++;
	}
	return x;
}
int adressing(char* x) {
	char *y;
	
	if (x==NULL) {
		return 0;
	}
	/*checks if the given argument is a register, 48 is '0' and 55 is '7'*/
	if (x[0]=='r' && 48<=x[1] && x[1]<=55 && (x[2]==0 || x[2]==' ')) {
		L+=1;
		
		return 3;/**/
	}
	/*checks if the given argument is a raw number*/
	if (x[0]=='#') {
		L+=1;
		if (check_number(x+1)==0) {
			
			ERROR=1;
			return -1;
		}
		return 0;/*direct number*/
	}
	y=strstr(x,".");
	/*if there is a . its a struct, else its a label*/
	if (!y){
		L+=1;
		
		return 1; /*label */
	}else {
		if (atoi(y+1)>=3 || atoi(y+1)<=0) {/*must be a number 1 or 2*/
			ERROR=1;
			return -1;
		}
	}
	L+=2;
	return 2;
	
}

int build_op_code(char *line) {
	char line_copy[LINESIZE];
	char *y;
	char *x;
	int code;
	int a;
	int b;
	b=how_many_arguments(line);
	y=strstr(line, ",");
	
	if (b>2) {/*too many arguments*/
		ERROR=4;
		return -1;
	}
	if (b<2 && y) {/*too many commas*/
		ERROR=8;
		return -1;
	}
	L=1;
	strcpy(line_copy,line);
	x=strstr(line_copy, " ");
	
	if (x!=NULL) {/*there is a space*/
		x=strtok(line_copy, " ");	
	}else{/*there isnt a space, so we have 0 operands*/
		x=line_copy;
		x[3]=' ';
			
		x=strtok(x," ");
		if (name_to_opcode(x)<0) {
			return -1;
		}
		return (name_to_opcode(x)<<6);/*only command opcode, else will be 0*/
		
	}
	
	code=name_to_opcode(x);/*gets opcode of command name*/
	
	if (code<0) {/*illegal command name*/
		
		ERROR=5;
		return -1;
	}
	
	code<<=2;
	x=strtok(NULL, ",");
	x=clear_spaces(x);
	a=adressing(x);/*שיטת מיעון של האופרנד הראשון*/
	if (a==-1) {
		
		return -1;
	}

	x=strtok(NULL,",");
	x=strtok(x," ");
	if (x==NULL) {
		if (b>1) {
			ERROR=4;
			return -1;
		}/*in case we only have one operand*/
		code<<=2;
		code+=a;
		code<<=2;
		return code;
	}
	
	code+=a;
	code<<=2;
	
	b=a;
	a=adressing(x);/*שיטת מיעון של האופרנד השני*/
	if (a==-1) {
		return -1;
	}
	
	if (a==3 && b==3) {/*if they're both registers, its only an additional one 				line of computer code*/
		L-=1;
	}
	code+=a;
	code<<=2;/*a.r.e is 0 (default)*/
	y=strstr(line, ",");
	y++;
	
	y=strstr(y, ",");
	if (y!=NULL) {/*too many arguments*/
		ERROR=8;
		return -1;	
	}
	
	return code;/*return opcode*/
	
	
}
int name_to_opcode(char *s) {
		
		
		if(strcmp("mov",s)==0) {
			return 0;
		}
		if(strcmp("cmp",s)==0) {
			return 1;
		}
		if(strcmp("add",s)==0) {
			return 2;
		}
		if(strcmp("sub",s)==0) {
			return 3;
		}
		if(strcmp("not",s)==0) {
			return 4;
		}
		if(strcmp("clr",s)==0) {
			return 5;
		}
		if(strcmp("lea",s)==0) {
			return 6;
		}
		if(strcmp("inc",s)==0) {
			return 7;
		}
		if(strcmp("dec",s)==0) {
			return 8;
		}
		if(strcmp("jmp",s)==0) {
			return 9;
		}
		if(strcmp("bne",s)==0) {
			return 10;
		}
		if(strcmp("get",s)==0) {
			return 11;
		}
		if(strcmp("prn",s)==0) {
			return 12;
		}
		if(strcmp("jsr",s)==0) {
			return 13;
		}
		if(strcmp("rts",s)==0) {
			return 14;
		}
		
		if(strcmp("hlt",s)==0) {
			return 15;
		}

	return -1;
}
int is_saved_word(char* s) {
	int i;
	
	for (i=0; i<RESERVEDWORDS; i++) {
		if (strcmp(s,saved_words[i])==0) {
			return 1;
		}
	}
	return 0;
}
int label_exists(char* word) {
	struct node *x=head;
	while (x!=NULL && size_of_data>=1) {
		if (strcmp(x->name,word)==0) {
			return 1; /*label exists already*/
		}
		x=x->next;	
	}
	return 0;
}
char* is_label(char* line) {
	char* label;
	char* colon;
	label=strtok(line," ");
	colon = strrchr(label,':');
	/*checks if the colon is the last charcter before a space*/
	if (colon==label+strlen(label)-1) {
		label[strlen(label)-1]='\0';
		strcpy(line,label);
		if (is_saved_word(line)==1) {/*label name is a saved word*/
			
			ERROR=3;
			/*TELL THE USER THERE IS AN ERROR*/
			return "err";
		}
		if (label_exists(line)==1 && second==0) {/*label name already exists*/
			
			ERROR=3;
			return "err";
		}
		return line;
	}
	return NULL;
}
int is_extern_or_entry(char* line) {
	char *pt;
	char copy[LINESIZE];
	int args;
	args= how_many_arguments(line);
	
	strcpy(copy,line);
	pt=strstr(line,".extern");

	if (pt==line) {/*the line starts with .extern*/
		if(args>=2) {
			ERROR=4;	
			return -1;
		}
		strtok(copy," ");/*gets label name*/
		pt=strtok(NULL," ");
      		pt=strtok(pt,",");
		
		if (pt==NULL) {/*no argument after .extern*/	
			ERROR=4;
			return -1;
		}
		if (second==0) {/*if we're in the first transition*/
			set_next(0, 1, pt, 0);
		}
		strcpy(copy,line);
		pt=strtok(copy,",");
		pt=strtok(NULL,",");
		if (pt!=NULL) {/*too many arguments*/
			ERROR=4;
			return -1;
		}
		return 1;
		
	}
	pt=strstr(line,".entry");
	if (pt==line) { /*Checks validity of .entry*/
		if(args>=2) {
			ERROR=4;	
			return -1;
		}
		strtok(copy," ");/*gets label name*/
		pt=strtok(NULL," ");
      		pt=strtok(pt,",");
		if (pt==NULL) {/*no argument after .entry*/
			ERROR=4;
			return -1;
		}

		strcpy(copy,line);
		pt=strtok(copy,",");
		pt=strtok(NULL,",");
		if (pt!=NULL) {/*too much arguments*/
			ERROR=4;
			return -1;
		}
		
		return 2;
	}
	return 0;
		
}
int is_empty_line(char* line) {
	char *x;
	x=strtok(line, " ");
	if (x==NULL || x[0]==';') {
		return 1;
	}
	return 0;
}
int is_storage(char* line) {
	char *pt;
	char copy[LINESIZE];
	int count;
	strcpy(copy,line);
	pt=strstr(line,".struct");

	if (pt==line) {/*if the line starts with .struct*/
		strtok(copy," ");
		pt=strtok(NULL," ");
      		pt=strtok(pt,",");
       		if (check_number(pt)==0) {/*NAN*/
			ERROR=1;
			
			return -1;
		}
		if (second==0) {
			set_next1(atof(pt),0);
		}

       		strcpy(copy,line);
       	 	pt=strtok(copy,",");
        	pt=strtok(NULL,",");
        	pt=strtok(pt," ");/*clears whitespaces*/
		if (pt[0]!='"' || pt[strlen(pt)-1]!='"') {/*invalid string*/
			ERROR=2;
			return -1;
			
		}
		if (second==0) {
			for (count=1; count<strlen(pt)-1; count++) {
				set_next1(pt[count],0);
				/*codes the charcters into binary code*/
			}
			set_next1(0,0);
			L=count+1;
		}
		return 1;
	}
	strcpy(copy,line);
	pt=strstr(line,".string");
	if (pt==line) {
		pt=strtok(copy," ");
        	pt=strtok(NULL," ");
	
		if (pt[0]!='"' || pt[strlen(pt)-1]!='"') {/*Invalid string*/
			
			ERROR=2;
			return -1;
			
		}
		if (second==0) {
		for (count=1; count<strlen(pt)-1; count++) {

			set_next1(pt[count],0);
			/*codes the charcters into binary code*/
		}
		set_next1(0,0);/*0 in the end as '/0'*/
		}
		L=count;

		
		return 2;
	}
	pt=strstr(line,".data");
	strcpy(copy,line);
	if (pt==line) {
		strtok(copy," ");
		pt=strtok(NULL," ");
      		pt=strtok(pt,",");
       		if (check_number(pt)==0) {/*NAN*/
			ERROR=1;
			return -1;
		}	
		if (second==0) {/*code the num into the memory unless we're already in 					the second transition*/
		set_next1(atoi(pt),0);
		}
		strcpy(copy,line);
		pt=strtok(copy,",");
		count=1;
		while(pt) {		
        		pt=strtok(NULL,",");
			if (pt==NULL) {
				break;
			}
        		if (check_number(pt)==0) {
				ERROR=1;
				return -1;
			}	
			if (second==0) {
			set_next1(atof(pt),0);/*code all the numbers into code*/
			}
			count++;
			
		}
		L=count;

		return 3;
	}
	return 0;
}

int check_number(char* text) {
	int i;
	int cont=0;
	for (i=0; i<strlen(text); i++) {
		if (text[i]==' ') {
	       	    continue;
		}
		
		if (!(isdigit(text[i]) || (cont==0 && (text[i]=='-' || text[i]=='+')))) {
			/*isnt a number*/
			return 0;
		}
		cont++;
	}
	return 1;
}
void print_error_message(int line_num, char* line) {
	if (ERROR==1) {
		printf("- Illegal number in line %d: %s\n",line_num, line);
	}
	if (ERROR==2) {
		printf("- Illegal string in line %d: %s\n", line_num,line);
	}
	if (ERROR==3) {
		printf("- Label name already occupied in line %d: %s\n", line_num, line);
	}
	if (ERROR==4) {
	       printf("- Illegal amount of arguments in line %d: %s\n", line_num, line);
	}
	if (ERROR==5) {
		printf("- Illegal command name in line %d: %s\n", line_num, line);
	}
	if (ERROR==6) {
		printf("- External variable being set to entry in line %d: %s\n", line_num, line);
	}
	if (ERROR==7) {
		printf("- A variable is being marked as entry which doesn't exist in line %d: %s\n", line_num, line);
	}
	if (ERROR==8) {
		printf("- Illegal comma in line %d: %s\n", line_num, line);
	}
	if(ERROR==9) {
		printf("- Unrecognized label name in line %d: %s\n",line_num,line);
	}
}


