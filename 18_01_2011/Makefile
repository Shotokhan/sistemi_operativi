all: loadBalance

loadBalance: main.o procedure.o
	gcc -o loadBalance main.o procedure.o

main.o: main.c header.h
	gcc -g -c main.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./loadBalance
