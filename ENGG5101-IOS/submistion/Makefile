all : simulator-interrupt.c 
	gcc -o icpu simulator-interrupt.c -Wall
	./assembler 4p-os.asm ios
clean : 
	rm icpu ios
debug : simulator-interrupt.c
	gcc -o icpu simulator-interrupt.c -D DEBUG
	./assembler 4p-os.asm ios
