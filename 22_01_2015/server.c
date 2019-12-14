#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

int main(){
	int shmid = shmget(ftok(KEY_PATH, KEY_CHAR_RISORSA), sizeof(Risorsa), 0);
	checkError(shmid, "Server- Errore ottenimento shared memory\n");
	Risorsa* r = (Risorsa*) shmat(shmid, NULL, 0);
	if(r == (void*) -1){
		checkError(-1, "Server - Errore shmat\n");
	}
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_QUEUE), 0);
	checkError(qid, "Server - Errore ottenimento coda\n");
	Messaggio msg;

	for(int i=0; i<4; i++){
		msgrcv(qid, (void*) &msg, sizeof(Messaggio)-sizeof(long), 0, 0); // receive bloccante
		sleep(1);
		enterMonitor(&(r->m));
		r->a = msg.a;
		r->b = msg.b;
		printf("\tServer %d: ho scritto %d, %d\n", getpid(), msg.a, msg.b);
		leaveMonitor(&(r->m));
	}
	_exit(0);
}	
