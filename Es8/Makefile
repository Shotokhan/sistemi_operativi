all: msgQueue

msgQueue: queue.o procedure.o client1 client2 server
	gcc -o msgQueue queue.o procedure.o
	rm -f *.o
	rm -f *.~

server: p3.o procedure.o
	gcc -o server p3.o procedure.o

client2: p2.o procedure.o
	gcc -o client2 p2.o procedure.o

client1: p1.o procedure.o
	gcc -o client1 p1.o procedure.o

queue.o: queue.c header.h
	gcc -g -c queue.c

p3.o: p3.c header.h
	gcc -g -c p3.c

p2.o: p2.c header.h
	gcc -g -c p2.c

p1.o: p1.c header.h
	gcc -g -c p1.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./msgQueue
	rm -f ./client1
	rm -f ./client2
	rm -f ./server
