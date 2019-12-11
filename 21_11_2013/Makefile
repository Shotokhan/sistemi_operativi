all: chat

chat: main.o procedure.o
	gcc -o chat main.o procedure.o

main.o: main.c header.h
	gcc -g -c main.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f ./chat
	rm -f *.o
	rm -f *.~
