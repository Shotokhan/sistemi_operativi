#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include "header.h"
#include "Server.h"

#define N_SERVICES 6

#define N_THREADS 2

int main(){
	pthread_t thread[N_THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	key_t keyval = ftok(KEY_PATH, KEY_CHAR);
	int msqid;
	if((msqid = getQueue(keyval)) == -1){
		_exit(1);
	}
	buffer tBuf[N_THREADS];
	initBuffer(tBuf, N_THREADS, msqid);
	for(int i = 0; i < N_THREADS; i++){
		pthread_create(&thread[i], &attr, serverWorker, (void*) &tBuf[i]);
	}
	int st;
	for(int i = 0; i < N_THREADS; i++){
		pthread_join(thread[i], (void**) &st);
		printf("Il thread %d ha terminato con stato %d\n", thread[i], st);
	}
	
	pthread_attr_destroy(&attr);
	_exit(0);
}

void* serverWorker(void* buf){
	buffer* myBuf = (buffer*) buf;
	printf("Thread %d in esecuzione\n", myBuf->threadId);
	data D;
	for(int i=0; i<N_SERVICES; i++){
		receiveRendezVous(myBuf->msqid, &D);
		printf("Thread %d: somma calcolata %d, client servito %d\n", myBuf->threadId, D.sum, D.pid);
	}
	pthread_exit(0);
}

void initBuffer(buffer buf[], int N, int msqid){
	for(int i = 0; i < N; i++){
		buf[i].threadId = i+1;
		buf[i].msqid = msqid;
	}
}
