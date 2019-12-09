all: treni

treni: main.o procedure.o
	gcc -pthread -o treni main.o procedure.o

main.o: main.c header.h
	gcc -g -c main.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f ./treni
	rm -f *.o
	rm -f *.~
