#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <pthread.h>

#include "header.h"

void Manager(workerData* W){
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_REQ), 0);
	checkError(qid, "Manager: Errore ottenimento coda\n");
	client_msg msg;

	pthread_t foo;

	while(1){
		msgrcv(qid, &msg, sizeof(client_msg)-sizeof(long), 0, 0);
		if(msg.val[0] == -1 && msg.val[1] == -1){
			break;
		}

		pthread_mutex_lock(&(W->mutex));
		while(W->consumato == 0){
			pthread_cond_wait(&(W->prod), &(W->mutex));
		}
		W->val[0] = msg.val[0];
		W->val[1] = msg.val[1];
		W->pid = msg.pid;
		W->consumato = 0;
		pthread_cond_signal(&(W->cons));
		pthread_mutex_unlock(&(W->mutex));

		pthread_create(&foo, NULL, Worker, (void*) W);	
		sleep(1);
	}	
}

void* Worker(void* data){
	workerData* W = (workerData*) data;
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_ANS), 0);
	checkError(qid, "Worker: Errore ottenimento coda\n");
	server_msg s_msg;

	pthread_mutex_lock(&(W->mutex));
	while(W->consumato == 1){
		pthread_cond_wait(&(W->cons), &(W->mutex));
	}
	s_msg.tipo = W->pid;
	s_msg.result = W->val[0] * W->val[1];
	W->consumato = 1;
	pthread_cond_signal(&(W->prod));
	pthread_mutex_unlock(&(W->mutex));

	msgsnd(qid, &s_msg, sizeof(server_msg)-sizeof(long), 0);
	pthread_exit(0);
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		pthread_exit(0);
	}
}

void initData(workerData* W){
	int r = pthread_mutex_init(&(W->mutex), 0);
	checkError(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(W->prod), 0);
	checkError(r, "Errore creazione condsem\n");
	r = pthread_cond_init(&(W->cons), 0);
	checkError(r, "Errore creazione condsem\n");
	W->consumato = 1;
}

void removeData(workerData* W){
	int r = pthread_mutex_destroy(&(W->mutex));
	checkError(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(W->prod));
	checkError(r, "Errore distruzione condsem\n");
	r = pthread_cond_destroy(&(W->cons));
	checkError(r, "Errore distruzione condsem\n");
}
