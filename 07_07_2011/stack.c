#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include "stack.h"

void StackInit(Stack* s, int dim){
	if(s->init == 1){
		printf("Stack già inizializzato. Impossibile modificarlo senza rimuoverlo\n");
		pthread_exit(0);
	}
	int r;
	r = pthread_mutex_init(&(s->mutex), 0);
	checkError(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(s->push), 0);
	checkError(r, "Errore creazione condsem\n");
	r = pthread_cond_init(&(s->pop), 0);
	checkError(r, "Errore creazione condsem\n");
	s->dati = (Elem*) malloc(sizeof(Elem)*dim);
	s->dim = dim;
	s->headPtr = 0;
	s->init = 1;
}

void StackRemove(Stack* s){
	int r;
	r = pthread_mutex_destroy(&(s->mutex));
	checkError(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(s->push));
	checkError(r, "Errore distruzione condsem\n");
	r = pthread_cond_destroy(&(s->pop));
	checkError(r, "Errore distruzione condsem\n");
	free(s->dati);
	s->init = 0;
}


// per le 3 prossime funzioni: precondition = stack inizializzato

void StackPush(Stack* s, Elem e){
	pthread_mutex_lock(&(s->mutex));
	while(StackSize(s) == s->dim){
		pthread_cond_wait(&(s->push), &(s->mutex));
	}
	s->dati[s->headPtr] = e;
	s->headPtr += 1;
	pthread_cond_signal(&(s->pop));
	pthread_mutex_unlock(&(s->mutex));
}

Elem StackPop(Stack* s){
	pthread_mutex_lock(&(s->mutex));
	while(StackSize(s) == 0){
		pthread_cond_wait(&(s->pop), &(s->mutex));
	}
	s->headPtr -= 1;
	Elem elem = s->dati[s->headPtr];
	pthread_cond_signal(&(s->push));
	pthread_mutex_unlock(&(s->mutex));
	return elem;
}

int StackSize(Stack* s){
	return s->headPtr;
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		pthread_exit(0);
	}
}
