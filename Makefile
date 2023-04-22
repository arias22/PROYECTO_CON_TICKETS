all: main_v3 receptor_v3  init_colas


main_testigo:
	gcc main_v3.c -o main_v3

receptor_testigo:
	gcc receptor_v3.c -o receptor_v3


init_colas: 
	gcc init_colas.c -o init_colas