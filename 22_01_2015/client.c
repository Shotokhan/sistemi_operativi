#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <time.h>

#include "header.h"

int main(){
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_QUEUE), 0);
	checkError(qid, "Client - Errore ottenimento coda\n");
	Messaggio msg;
	srand(time(NULL)*getpid());
	// il campo tipo non m'interessa, i server ricevono in modo FIFO
	msg.tipo = 10;

	for(int i=0; i<4; i++){
		msg.a = rand() % 10;
		msg.b = rand() % 10;
		msgsnd(qid, (void*) &msg, sizeof(Messaggio)-sizeof(long), 0); // send asincrona
		printf("Client %d: ho inviato %d, %d\n", getpid(), msg.a, msg.b);
		sleep(rand() % 2 + 1);
	}
	_exit(0);
}
