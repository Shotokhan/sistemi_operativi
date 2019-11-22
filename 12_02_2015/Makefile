all: reqElab

reqElab: main.o procedure.o
	gcc -pthread -o reqElab main.o procedure.o
	rm -f *.o
	rm -f *.~

main.o: main.c procedure.h
	gcc -g -c main.c

procedure.o: procedure.c procedure.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./reqElab
