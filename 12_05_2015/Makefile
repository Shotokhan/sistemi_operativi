all: gestioneVoli

gestioneVoli: main.o procedure.o
	gcc -pthread -o gestioneVoli main.o procedure.o

main.o: main.c procedure.h
	gcc -g -c main.c

procedure.o: procedure.c procedure.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./gestioneVoli
