assem: preassem.c data.h data.c transition1.c transition1.h transition2.c
	gcc -g preassem.c data.c transition1.c transition2.c -ansi -Wall -pedantic -o assem
clean:
	 rm -f *.ob
	 rm -f *.ent
	 rm -f *.ext
	 rm -f *.am
	

