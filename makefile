all: libuthread.a
	gcc -o test test.c -L../GerenciadorThread/  -luthread -Wall

libuthread.a: tid.o uthread.o
	ar crs libuthread.a tid.o uthread.o

tid.o: tid.c
	gcc -c tid.c -Wall

uthread.o: uthread.c
	gcc -c uthread.c -Wall

clean:
	rm -rf *.a *.o test
