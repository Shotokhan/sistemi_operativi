all: master

master: master.o procedure.o produttore consumatore
	gcc -lm -o master master.o procedure.o

produttore: produttore.o procedure.o
	gcc -lm -o produttore produttore.o procedure.o

consumatore: consumatore.o procedure.o
	gcc -lm -o consumatore consumatore.o procedure.o

master.o: master.c procedure.h
	gcc -g -c master.c

produttore.o: produttore.c procedure.h
	gcc -g -c produttore.c

consumatore.o: consumatore.c procedure.h
	gcc -g -c consumatore.c

procedure.o: procedure.c procedure.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./consumatore
	rm -f ./produttore
	rm -f ./master
