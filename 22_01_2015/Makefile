all: control

control: main.o procedure.o client server
	gcc -o control main.o procedure.o

client: client.o procedure.o
	gcc -o client client.o procedure.o

server: server.o procedure.o
	gcc -o server server.o procedure.o

main.o: main.c header.h
	gcc -g -c main.c

client.o: client.c header.h
	gcc -g -c client.c

server.o: server.c header.h
	gcc -g -c server.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f ./control
	rm -f ./client
	rm -f ./server
	rm -f *.o
	rm -f *.~
