#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "header.h"

void sendSincrona(int msqid, data* D){
	synch S;
	S.tipoMsg = REQUEST_TO_SEND;
	S.pid = D->pid;
	msgsnd(msqid, (void*) &S, sizeof(synch)-sizeof(long), 0);
	msgrcv(msqid, (void*) &S, sizeof(synch)-sizeof(long), S.pid, 0);
	msgsnd(msqid, (void*) D, sizeof(data)-sizeof(long), 0);
	msgrcv(msqid, (void*) D, sizeof(data)-sizeof(long), S.pid, 0);
}

void receiveRendezVous(int msqid, data* D){
	synch S;
	msgrcv(msqid, (void*) &S, sizeof(synch)-sizeof(long), REQUEST_TO_SEND, 0);
	S.tipoMsg = S.pid;
	// questo è l'OK_TO_SEND
	msgsnd(msqid, (void*) &S, sizeof(synch)-sizeof(long), 0);
	msgrcv(msqid, (void*) D, sizeof(data)-sizeof(long), S.pid, 0);
	D->sum = D->val[0] + D->val[1];
	msgsnd(msqid, (void*) D, sizeof(data)-sizeof(long), 0);
}

int openQueue(key_t keyval){
	int qid;
	if((qid = msgget(keyval, IPC_CREAT|0664)) == -1){
		perror("Creazione coda fallita\n");
		return -1;
	}
	return qid;
}

int getQueue(key_t keyval){
	int qid;
	if((qid = msgget(keyval, 0)) == -1){
		perror("Richiesta coda fallita\n");
		return -1;
	}
	return qid;
}

void removeQueue(int msqid){
	msgctl(msqid, IPC_RMID, 0);
}
