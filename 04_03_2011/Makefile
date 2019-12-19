all: control

control: main.o client server
	gcc -o control main.o

server: server.o procedure.o
	gcc -pthread -o server server.o procedure.o

client: client.o
	gcc -o client client.o

main.o: main.c header.h mex.h
	gcc -g -c main.c

client.o: client.c mex.h
	gcc -g -c client.c

server.o: server.c header.h mex.h
	gcc -g -c server.c

procedure.o: procedure.c header.h mex.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./control
	rm -f ./server
	rm -f ./client
