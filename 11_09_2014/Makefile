all: key_value

key_value: main.o procedure.o
	gcc -g -o key_value main.o procedure.o
	rm -f *.o
	rm -f *.~

main.o: main.c header.h
	gcc -g -c main.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./key_value
