all: calcParallelo

calcParallelo: main.o procedure.o children
	gcc -o calcParallelo main.o procedure.o

main.o: main.c header.h
	gcc -g -c main.c

children: children_out
	for i in {1..6}; do \
		gcc -o p$$i p$$i.o procedure.o ; \
	done

children_out: header.h
	for i in {1..6}; do \
		gcc -g -c p$$i.c ; \
	done

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	for i in {1..6}; do \
		rm -f ./p$$i ; \
	done
	rm -f ./calcParallelo
