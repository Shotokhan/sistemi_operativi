all: scheduler

scheduler: main.o proc.o monitorHoare.o
	gcc -o scheduler main.o proc.o monitorHoare.o

main.o: main.c proc.h header.h
	gcc -g -c main.c

proc.o: proc.c proc.h header.h
	gcc -g -c proc.c

monitorHoare.o: monitorHoare.c header.h
	gcc -g -c monitorHoare.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./scheduler
