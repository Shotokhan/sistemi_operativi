all: stack

stack: main.o test.o stack.o
	gcc -pthread -o stack main.o test.o stack.o

main.o: main.c test.h stack.h
	gcc -g -c main.c

test.o: test.c test.h stack.h
	gcc -g -c test.c

stack.o: stack.c stack.h
	gcc -g -c stack.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./stack
