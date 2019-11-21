all: PriorityProdCons

PriorityProdCons: main.o procedure.o monitor.o
	gcc -o PriorityProdCons main.o procedure.o monitor.o
	rm -f *.o
	rm -f *.~

main.o: main.c procedure.h header.h
	gcc -g -c main.c

procedure.o: procedure.c procedure.h header.h
	gcc -g -c procedure.c

monitor.o: monitor.c header.h
	gcc -g -c monitor.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./PriorityProdCons
