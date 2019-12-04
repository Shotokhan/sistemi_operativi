#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "header.h"

void client(int qidClient){
	int r;
	Msg clientMsg;
	clientMsg.tipo = getpid();
	clientMsg.data = clientMsg.tipo;
	r = msgsnd(qidClient, &clientMsg, sizeof(Msg)-sizeof(long), 0);
	checkError(r, "Client - Errore invio messaggio\n");
}

void balancer(int qidClient, int qidBalance, int qidServer[]){
	int r;
	Msg routeMsg, counter;
	r = msgrcv(qidClient, &routeMsg, sizeof(Msg)-sizeof(long), 0, 0);
	checkError(r, "Balancer - Errore ricezione messaggio dal client\n");
	r = msgrcv(qidBalance, &counter, sizeof(Msg)-sizeof(long), 0, 0);
	checkError(r, "Balancer - Errore ricezione messaggio dalla coda balancer\n");
	r = msgsnd(qidServer[counter.data], &routeMsg, sizeof(Msg)-sizeof(long), 0);
	checkError(r, "Balancer - Errore invio messaggio al server\n");
//	printf("Balancer %d: invio messaggio a %d\n", getpid(), qidServer[counter.data]);
	counter.data = (counter.data + 1) % N_SERVER;
	r = msgsnd(qidBalance, &counter, sizeof(Msg)-sizeof(long), 0);
	checkError(r, "Balancer - Errore invio messaggio sulla coda balancer\n");
}

void server(int qidServer){
	int r;
	Msg serverMsg;
	r = msgrcv(qidServer, &serverMsg, sizeof(Msg)-sizeof(long), 0, 0);
	checkError(r, "Server - Errore ricezione messaggio\n");
	printf("Server %d: ricevuto messaggio dal client %d\n", getpid(), serverMsg.data);
}

void initQueues(int* qidClient, int* qidBalance, int qidServer[]){
	int r;
	r = msgget(ftok(KEY_PATH, KEY_CHAR_1), IPC_CREAT|0666);
	checkError(r, "Errore creazione coda\n");
	*qidClient = r;
	r = msgget(ftok(KEY_PATH, KEY_CHAR_2), IPC_CREAT|0666);
	checkError(r, "Errore creazione coda\n");
	*qidBalance = r;
	for(int i=0; i<N_SERVER; i++){
		r = msgget(ftok(KEY_PATH, KEY_CHAR_3 + i), IPC_CREAT|0666);
		checkError(r, "Errore creazione coda\n");
		qidServer[i] = r;
	}
	Msg counter;
	counter.tipo = getpid();
	counter.data = 0;
	r = msgsnd(*qidBalance, &counter, sizeof(Msg)-sizeof(long), 0);
	checkError(r, "Main - Errore invio messaggio\n");
}

void removeQueues(int qidClient, int qidBalance, int qidServer[]){
	int r;
	r = msgctl(qidClient, IPC_RMID, 0);
	checkError(r, "Errore rimozione coda\n");
	r = msgctl(qidBalance, IPC_RMID, 0);
	checkError(r, "Errore rimozione coda\n");
	for(int i=0; i<N_SERVER; i++){
		r = msgctl(qidServer[i], IPC_RMID, 0);
		checkError(r, "Errore rimozione coda\n");
	}
}

void checkError(int retVal, char* msg){
	if(retVal < 0){
		perror(msg);
		_exit(1);
	}
}
