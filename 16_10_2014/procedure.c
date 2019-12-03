#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

void genera(monitorVettore* v, elemento e){
	pthread_mutex_lock(&(v->mutex));
	while(v->numLiberi == 0){
		pthread_cond_wait(&(v->prod), &(v->mutex));
	}
	v->numLiberi -= 1;

	v->vettore[v->testa].a = e.a;
	v->vettore[v->testa].b = e.b;
	v->testa = (v->testa + 1) % DIM;

	v->numOccupati += 1;
	pthread_cond_signal(&(v->cons));
	pthread_mutex_unlock(&(v->mutex));
}

elemento preleva(monitorVettore* v){
	elemento e;
	pthread_mutex_lock(&(v->mutex));
	while(v->numOccupati == 0){
		pthread_cond_wait(&(v->cons), &(v->mutex));
	}
	v->numOccupati -= 1;

	e.a = v->vettore[v->coda].a;
	e.b = v->vettore[v->coda].b;
	v->coda = (v->coda + 1) % DIM;

	v->numLiberi += 1;
	pthread_cond_signal(&(v->prod));
	pthread_mutex_unlock(&(v->mutex));

	return e;
}

void aggiorna(monitorBuffer* b, elemento e){
	pthread_mutex_lock(&(b->mutex));
	while(b->numLettori > 0){
		pthread_cond_wait(&(b->scrittore), &(b->mutex));
	}
	b->numScrittori += 1;
	
	b->buffer.a = e.a;
	b->buffer.b = e.b;

	b->numScrittori -= 1;
	pthread_cond_signal(&(b->lettore));
	pthread_mutex_unlock(&(b->mutex));
}

void consulta(monitorBuffer* b){
	pthread_mutex_lock(&(b->mutex));
	while(b->numScrittori > 0){
		pthread_cond_wait(&(b->lettore), &(b->mutex));
	}
	b->numLettori += 1;
	pthread_cond_signal(&(b->lettore));
	pthread_mutex_unlock(&(b->mutex));	

	int sum = b->buffer.a + b->buffer.b;
	printf("Consultazione: %d + %d = %d\n", b->buffer.a, b->buffer.b, sum);

	pthread_mutex_lock(&(b->mutex));
	b->numLettori -= 1;
	pthread_cond_signal(&(b->scrittore));
	pthread_mutex_unlock(&(b->mutex));
}

void initMonitorVettore(monitorVettore* v){
	pthread_mutex_init(&(v->mutex), 0);
	pthread_cond_init(&(v->prod), 0);
	pthread_cond_init(&(v->cons), 0);
	v->numLiberi = DIM;
	v->numOccupati = 0;
	v->testa = 0;
	v->coda = 0;
	for(int i=0; i<DIM; i++){
		v->vettore[i].a = 0;
		v->vettore[i].b = 0;
	}
}

void initMonitorBuffer(monitorBuffer* v){
	pthread_mutex_init(&(v->mutex), 0);
	pthread_cond_init(&(v->lettore), 0);
	pthread_cond_init(&(v->scrittore), 0);
	v->buffer.a = 0;
	v->buffer.b = 0;
	v->numLettori = 0;
	v->numScrittori = 0;
}

void freeMonitorVettore(monitorVettore* v){
	pthread_mutex_destroy(&(v->mutex));
	pthread_cond_destroy(&(v->prod));
	pthread_cond_destroy(&(v->cons));
}

void freeMonitorBuffer(monitorBuffer* v){
	pthread_mutex_destroy(&(v->mutex));
	pthread_cond_destroy(&(v->lettore));
	pthread_cond_destroy(&(v->scrittore));
}

void* generatore(void* buf){
	bufGeneratore* data = (bufGeneratore*) buf;
	elemento e;
	for(int i=0; i<10; i++){
		e.a = rand() % 11;
		e.b = rand() % 11;
		printf("Sono il thread generatore %d, mi appresto a generare: %d %d\n", data->threadId, e.a, e.b);
		genera(data->v, e);
		printf("Sono il thread generatore %d, ho finito di generare: %d %d\n", data->threadId, e.a, e.b);
	}
	pthread_exit(0);
}

void* aggiornatore(void* buf){
	bufAggiornatore* data = (bufAggiornatore*) buf;
	elemento e;
	for(int i=0; i<10; i++){
		e = preleva(data->v);
		printf("Sono il thread aggiornatore %d, ho prelevato: %d %d\n", data->threadId, e.a, e.b);
		aggiorna(data->b, e);
		printf("Sono il thread aggiornatore %d, ho aggiornato il buffer: %d %d\n", data->threadId, e.a, e.b);
		sleep(1);
	}
	pthread_exit(0);
}

void* destinatario(void* buf){
	bufDestinatario* data = (bufDestinatario*) buf;
	for(int i=0; i<6; i++){
		printf("Sono il thread destinatario %d, mi appresto a consultare il buffer\n", data->threadId);
		consulta(data->b);
		printf("Sono il thread destinatario %d, ho finito di consultare il buffer\n", data->threadId);
		sleep(2);
	}
	pthread_exit(0);
}
