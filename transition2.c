#include "data.h"
#include "transition1.h"
#define EXTERN_SIZE 20
int move;/*the amount of offset(>>) thats needed for a command with registers, if there is only one then we need 6, otherwise we need only 2*/
int index1;/*index of the externs array*/
char externs[EXTERN_SIZE][2][10];/*stores externs, their name and address in 32 bit*/

int transition2() {
	FILE *fp;
	char line_copy[LINESIZE];
	char line[LINESIZE];
	char *after_label;
	int line_number=0;
	int label_on=0;
	int storage_type=0;
	int i;
	IC=0;
	second=1;
	index1=0;
	for (i=0; i<EXTERN_SIZE; i++) {
		externs[i][0][0]='\0';/*clears externs*/
		externs[i][1][0]='\0';
	}
	
	code_current=code_head;
	fp=fopen(newfilename,"r");
	if (fp==NULL) {
		fprintf(stderr,"File does not exist!");
		exit(1);	
	}
	while(fgets(line,LINESIZE,fp)!=NULL) {
		
		L=0;
		label_on=0;
		while(code_current && code_current->next && code_current->type==0) {
			
			/*skips over all the data code lines */
			code_current=code_current->next;
		}

		line[strlen(line)-1]='\0';
		
		line_number++;
		
		if (ERROR>0) {
			
			ERROR=-999;/*there was an error */
		}
		strcpy(line_copy,line);
		
		if (is_empty_line(line_copy)==1) {
			
			continue;
		}
		
		after_label=line_copy;
		
		after_label=is_label(after_label);
		strcpy(line_copy,line);
		if (after_label!=NULL) {/*there is a label*/
			
			label_on=1;
		}
		
		if (label_on==1) {/*gets the string from after the label*/
			after_label=strchr(line_copy,':')+1;
			after_label=clear_spaces(after_label);
			
		}else{
			after_label=line_copy;
		}
		
		storage_type=is_storage(after_label);
		if (storage_type>0) {
			/*continues if its a data command such as .data*/
			continue;
		}
		
		storage_type=is_extern_or_entry(after_label);
		
		if (storage_type==1) {/*cotinues if its .extern*/
			continue;
		}
		
		if (storage_type==2) {
			/*marks the needed label as entry*/
			if (mark_entry(after_label)==-1) {
				print_error_message(line_number,line);	
			}
			
			continue;
		}
		/*fills in the missing lines of code*/
		if (finalize_op_code(after_label,code_current->code)==-1) {
			
			print_error_message(line_number,line);
			
			IC+=L;			
			continue;
		}
		IC+=L;		
	}
	if (ERROR!=0) {
		free_nodes();/*frees the linkes list in case of an error*/
		free_nodes1();
		return -1;
	}
	
	create_files();/*creates files accordingly*/
	return 0;
	
}
int mark_entry(char* line) {
	struct node *x=head;
	char *y;
	int i;
	y=strstr(line, ".entry");
	y+=strlen(".entry ");
	y=clear_spaces(y);/*gets stuff after .entry*/
	
	for(i=0; i<strlen(y); i++) {
		if (y[i]==' ') {
			y[i]='\0';
			break;
		}
	}
	while (x) {
		if (strcmp(x->name,y)==0) {
			if (x->type==1 || x->type==0) {
			 /*if the entry label is already external*/
				ERROR=6;
				return -1;
			}
			x->type=0;
			return 0;
		}
		x=x->next;
	}
	ERROR=7;/*no label to mark as entry*/
	return -1;
}

int how_many_arguments(char* line) {
	
	int space_on=0;
	int char_on=0;
	int i;
	int count=0;
	line = clear_spaces(line);
	for (i=0; i<strlen(line); i++) {
		if (line[i]!=' ' && line[i]!=',') {
			char_on=1;
			if (char_on && space_on) {
				count++;
			}
			space_on=0;
		}else{
			char_on=0;
			space_on=1;
		}
	}	
	
	return count;
	
}
int create_files() {
	
	char *s1, *s2;
	FILE *obj;
	FILE *ent;
	FILE *ext;
	char copy[LINESIZE];
	char* tok=".am";
	char* tok2=".ent";
	char* tok3=".ext";
	char* tok4=".ob";
	int i;
	char *name;
	current=head;
	name=strtok(newfilename,tok);/*name of file*/
	strcpy(copy,name);
	s1=malloc(sizeof(char)*3);
	s2=malloc(sizeof(char)*3);
	
	for (i=0; i<EXTERN_SIZE; i++) {
		if (externs[i][0][0]==0) {/*break when we dont have any more externs*/
			break;
		}
		
		if (i==0) {
			/*create the file for the first time*/
			ext=fopen(strcat(copy,tok3),PERMS);
		}
		/*prints the extern into the externs file*/
		fprintf(ext,"%s\n",strcat(strcat(externs[i][0],"\t"),externs[i][1]));
	}
	
	strcpy(copy,name);
	if (i!=0) {
		/*if the file exists*/
		fclose(ext);
	}else{
		/*deletes the old file of .ext in case it exists*/
		remove(strcat(copy,tok3));
	}
	strcpy(copy,name);
	i=0;
	
	
	while (current->next) {
		if (current->type==0) {
			if (i==0) {
				/*create the file for the first time*/
				ent=fopen(strcat(copy,tok2),PERMS);
			}
			
			s1=get_num_coverted(current->address,s1);
			/*Print the entry name + address in the .ent file*/
			fprintf(ent,"%s\n",strcat(strcat(current->name,"\t"),s1));
			i++;
		}
		current=current->next;
	}
	
	strcpy(copy,name);
	
	if (i!=0) {
		fclose(ent);
	}else{
		/*deletes the old file of .ent in case it exists*/
		remove(strcat(copy,tok2));
	}
	strcpy(copy,name);
	code_current=code_head;
	i=100;
	
	obj=fopen(strcat(copy,tok4),PERMS);
	/*prints IC, DC into the file's first line*/
	s1=get_num_coverted(IC,s1);
	s2=get_num_coverted(DC,s2);
	fprintf(obj,"   %s %s\n",s1,s2);
	
	while (code_current) {
		/*prints the command orders first into the file*/
		if (code_current->type==1) {
		s1=get_num_coverted(i,s1);/*line address*/
		s2=get_num_coverted(code_current->code,s2);/*code*/
		fprintf(obj,"%s\t%s\n",s1,s2);
		i++;
		
		}
		code_current=code_current->next;
	}
	code_current=code_head;
	while (code_current) {
		/*prints the data into the file*/
		if (code_current->type==0) {
		s1=get_num_coverted(i,s1);
		s2=get_num_coverted(code_current->code,s2);
		fprintf(obj,"%s\t%s\n",s1,s2);
		i++;
		
		}
		code_current=code_current->next;
	}
	/*frees everything*/
	free(s1);
	free(s2);
	
	free_nodes();
	free_nodes1();
	
	return 0;
}
int code_lines(char *x, int a) {
	/*a is the שיטת מיעון*/
	char *y;
	char copy[LINESIZE];
	int num;
	struct node *t=head;
	
	if (a==0) {/*מיעון מיידי*/
		y=strstr(x,"#");
		y++;
		num=atoi(y);
		if (num<0) {/*complement 2 system*/
			num=256+num;
		}
		num=num<<2; /*add A,R,E which is 0 for a number*/
		L+=1;
		code_current->code=num;
		code_current=code_current->next;
		return 0;
	}
	if (a==1) {/*מיעון ישיר*/
	
	while (t) {/*go over the labels*/
		
		if (strcmp(t->name,x)==0) {
			if (t->type==1) {

				char *s=malloc(sizeof(char)*3);
				s=get_num_coverted(IC+100+L,s);
				/*enterns externs into the array*/
				strcpy(externs[index1][0],t->name);
				strcpy(externs[index1][1],s);
				index1++;
			}
			/*gets the current line code+A.R.E*/
			code_current->code=((t->address)<<2)+(t->type);
			code_current=code_current->next;
			L+=1;
			return 0;
		}
		t=t->next;
	}
	
	code_current=code_current->next;
	
	ERROR=9; /*label not found*/
	L+=1;
	
	return -1;
	}
	if (a==2) {/*מיעון גישה לרשומה*/
		strcpy(copy,x);
		y=strstr(x,".");
		x[strlen(x)-strlen(y)]='\0';
				
		while (t) {/*go over the labels*/
		
			if (strcmp(t->name,x)==0) {
				if (t->type==1) {
					/*enterns externs into the array*/
					char *s=malloc(sizeof(char)*3);
					s=get_num_coverted(IC+100+L,s);
					strcpy(externs[index1][0],t->name);
					strcpy(externs[index1][1],s);
					index1++;
				}
				/*gets the current line code+A.R.E*/
				code_current->code=((t->address)<<2)+(t->type);
				code_current=code_current->next;
				y=strstr(copy,".");
				/*gets the number after .*/
				code_current->code=(atoi(y+1)<<2);
				code_current=code_current->next;
				L+=2;
				return 0;
			}
			t=t->next;
		}
		/*we go to the next 2 nodes that if we have an error, we'll stil be able to output more real errors, and not getting the file lines confused with the 		computer lines*/
		code_current=code_current->next;
		code_current=code_current->next;
		L+=2;
		ERROR=9; /*label not found*/
		return -1;
	}	
	if (a==3) {/*מיעון לרגיסטר*/
		code_current->code=(atoi(x+1)<<move);
		code_current=code_current->next;
		
		L+=1;
	}
	
	return 0;
	
}




int finalize_op_code(char *line, int code) {
	
	char copy[LINESIZE];
	char *x;
	int maskI=63;/*111111 in binary*/
	int maskII=15; /*1111 in binary */
	int sort;
	int sort2=0;
	int args;
	int num=0;
	L=1;
	move=6;
	args=how_many_arguments(line);
	
	code_current=code_current->next;
	strcpy(copy,line);
	
	if (args==0) {/*no more computer code lines*/
		
		return 0;
	}
	if (args==1) {
		
		x=strtok(copy," ");
		x=strtok(NULL," ");
		
		/*gets the first REAL A.R.E*/
		code&=maskII;
		sort=code>>2;
		if (code_lines(x,sort)==-1) {
			return -1;
		}
		
	}else{
		
		x=strtok(copy," ");
		x=strtok(NULL,",");
		/*gets the first A.R.E*/
		code&=maskI;
	
		sort=code>>4;
		/*gets the second A.R.E*/
		code&=maskII;
		sort2=code>>2;
		
		if (sort==3 && sort2==3) {
			/*in case of 2 registers we get both numbers */	
			num+=atoi(x+1);
			num<<=4;
			x=strtok(NULL,",");
			x=clear_spaces(x);
			num+=atoi(x+1);
			num<<=2;
			code_current->code=num;
			code_current=code_current->next;
			L+=1;
			return 0;
		}
		
		if (code_lines(x,sort)==-1) {
			num=-1;/*ERROR*/
		}
		
		x=strstr(line,",");/*find ,*/
		
		x++;
		
		x=clear_spaces(x);
		move-=4;
		
		if (code_lines(x,sort2)==-1) {
			num =-1;/*ERROR*/
		}
		
		return num;
	}
	
	x=clear_spaces(x);
	return 0;
}



