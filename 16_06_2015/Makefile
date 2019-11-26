all: control

control: control.o procedure.o Client Server
	gcc -o control control.o procedure.o

Server: Server.o procedure.o
	gcc -pthread -o Server Server.o procedure.o

Client: Client.o procedure.o
	gcc -o Client Client.o procedure.o

control.o: control.c header.h
	gcc -g -c control.c

Server.o: Server.c header.h Server.h
	gcc -g -c Server.c

Client.o: Client.c header.h
	gcc -g -c Client.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f *.~
	rm -f ./control
	rm -f ./Client
	rm -f ./Server
