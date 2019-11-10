all: control

control: control.o procedure.o docente studente
	gcc -g -o control control.o procedure.o
	rm -f *.o
	rm -f *.~

docente: docente.o procedure.o
	gcc -g -o docente docente.o procedure.o

studente: studente.o procedure.o
	gcc -g -o studente studente.o procedure.o

control.o: control.c header.h
	gcc -g -c control.c

docente.o: docente.c header.h
	gcc -g -c docente.c

studente.o: studente.c header.h
	gcc -g -c studente.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f ./control
	rm -f ./docente
	rm -f ./studente
	rm -f *.~
