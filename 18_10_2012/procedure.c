#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

void initQueues(int* qid, int nQueues){
	for(int i=0; i < nQueues; i++){
		qid[i] = msgget(ftok(KEY_PATH, KEY_CHAR + i + 1), IPC_CREAT|0666);
		checkError(qid[i], "Errore creazione coda\n");
	}
}

void removeQueues(int* qid, int nQueues){
	int r;
	for(int i=0; i < nQueues; i++){
		r = msgctl(qid[i], IPC_RMID, 0);
		checkError(r, "Errore eliminazione coda\n");
	}
}

void checkError(int retVal, char* msg){
	if(retVal < 0){
		perror(msg);
		_exit(1);
	}
}

void waitProcesses(){
	int pid, st;
	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if (WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));		
		}
	}
}

void msgGen(msgCalc* msg, int qid, int nOps, char* opName, int* count){
	msg->tipo = OP;
	int op, r;
	for(int j=0; j < nOps; j++){
		op = rand() % 10 + 1;
		printf("Operando generato: %c = %d\n", opName[*count], op);
		msg->operandi[j] = op;
		(*count) += 1;
	}
	msg->numOperandi = nOps;
	r = msgsnd(qid, msg, sizeof(msgCalc)-sizeof(long), 0);
	checkError(r, "Errore invio messaggio\n");
}

void opSend(msgCalc* msg, int qid, int nOps, int* operandi){
	msg->tipo = OP;
	for(int i=0; i<nOps; i++){
		msg->operandi[i] = operandi[i];
	}
	msg->numOperandi = nOps;
	int r;
	r = msgsnd(qid, msg, sizeof(msgCalc)-sizeof(long), 0);
	checkError(r, "Errore invio messaggio\n");
}
