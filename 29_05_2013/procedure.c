#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <pthread.h>

#include "header.h"

void initBuffer(buffer* b){
	int r;
	r = pthread_mutex_init(&(b->mutex), 0);
	checkError(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(b->manager), 0);
	checkError(r, "Errore creazione cond\n");
	r = pthread_cond_init(&(b->worker), 0);
	checkError(r, "Errore creazione cond\n");
	for(int i=0; i<DIM; i++){
		b->stato[i] = LIBERO;
	}
	b->numOccupati = 0;
	b->numLiberi = DIM;
}

void removeBuffer(buffer* b){
	int r;
	r = pthread_mutex_destroy(&(b->mutex));
	checkError(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(b->manager));
	checkError(r, "Errore distruzione cond\n");
	r = pthread_cond_destroy(&(b->worker));
	checkError(r, "Errore distruzione cond\n");
}

void manager_produzione(buffer* b, client_msg m){
	pthread_mutex_lock(&(b->mutex));
	while(b->numLiberi == 0){
		pthread_cond_wait(&(b->manager), &(b->mutex));
	}
	int i = 0;
	while(i < DIM && b->stato[i] != LIBERO){
		i++;
	}
	if(i == DIM){
		printf("Manager - errore di sincronizzazione\n");
		pthread_exit(0);
	}
	b->stato[i] = IN_USO;
	b->numLiberi -= 1;
	pthread_mutex_unlock(&(b->mutex));

	b->buf[i].tipo = m.tipo;
	b->buf[i].val[0] = m.val[0];
	b->buf[i].val[1] = m.val[1];
	b->buf[i].pid = m.pid;

	pthread_mutex_lock(&(b->mutex));
	b->stato[i] = OCCUPATO;
	b->numOccupati += 1;
	pthread_cond_signal(&(b->worker));
	pthread_mutex_unlock(&(b->mutex));
}

void worker_consumo(buffer* b, client_msg* m){
	pthread_mutex_lock(&(b->mutex));
	while(b->numOccupati == 0){
		pthread_cond_wait(&(b->worker), &(b->mutex));
	}
	int i = 0;
	while(i < DIM && b->stato[i] != OCCUPATO){
		i++;
	}
	if(i == DIM){
		printf("Worker - errore di sincronizzazione\n");
		pthread_exit(0);
	}
	b->stato[i] = IN_USO;
	b->numOccupati -= 1;
	pthread_mutex_unlock(&(b->mutex));

	m->tipo = b->buf[i].tipo;
	m->val[0] = b->buf[i].val[0];
	m->val[1] = b->buf[i].val[1];
	m->pid = b->buf[i].pid;

	pthread_mutex_lock(&(b->mutex));
	b->stato[i] = LIBERO;
	b->numLiberi += 1;
	pthread_cond_signal(&(b->manager));
	pthread_mutex_unlock(&(b->mutex));
}

void* Manager(void* data){
	managerData* M = (managerData*) data;
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_REQ), 0);
	checkError(qid, "Manager: Errore ottenimento coda\n");
	client_msg msg;
	int r;
	while(1){
		r = msgrcv(qid, &msg, sizeof(client_msg)-sizeof(long), 0, IPC_NOWAIT);
		if(r != -1){
			if(msg.val[0] == -1 && msg.val[1] == -1){
				break;
			}
			manager_produzione(M->b, msg);
		}
		sleep(1);
	}
	for(int i=0; i<N_WORKERS; i++){
		pthread_cancel(M->workers[i]);
	}
	pthread_exit(0);
}

void* Worker(void* data){
	buffer* b = (buffer*) data;
	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_ANS), 0);
	checkError(qid, "Worker: Errore ottenimento coda\n");
	client_msg c_msg;
	server_msg s_msg;
	while(1){
		worker_consumo(b, &c_msg);
		s_msg.tipo = c_msg.pid;
		s_msg.result = c_msg.val[0] * c_msg.val[1];
		msgsnd(qid, &s_msg, sizeof(server_msg)-sizeof(long), 0);
	}
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		pthread_exit(0);
	}
}
