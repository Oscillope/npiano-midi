CFLAGS = -lcurses -lasound
default: program
program: main.c
	gcc $(CFLAGS) main.c -o mnpiano
clean:
	-rm -f mnpiano
