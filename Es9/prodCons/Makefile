all: prodConsMT

prodConsMT: main.o procedure.o
	gcc -pthread -o prodConsMT main.o procedure.o
	rm -f *.o
	rm -f *.~

main.o: main.c header.h
	gcc -g -c main.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./prodConsMT
