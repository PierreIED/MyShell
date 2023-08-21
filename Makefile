all : monShell external

monShell : shell-final.c util.c
	gcc -g -Wall shell-final.c -lreadline util.c -o monShell
	

external : monkill monsleep monMan monCp

monMan : external/monMan.c
	gcc -g -Wall external/monMan.c -o external/monMan

monCp : external/monCp.c
	gcc -g -Wall external/monCp.c -o external/monCp

monkill : external/monkill.c
	gcc -g -Wall external/monkill.c -o external/monkill

monsleep : external/monsleep.c
	gcc -g -Wall external/monsleep.c external/f-monsleep.c -o external/monsleep

cleanAll :
	rm -r *.o
