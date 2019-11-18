#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

void* Produttore(void* heap){
	buff* b = (buff*) heap;
	msg mx = rand() % 100;
	Produci(b, mx);
	pthread_exit(NULL);
}

void* Consumatore(void* heap){
	buff* b = (buff*) heap;
	Consuma(b);
	pthread_exit(NULL);
}

// Precondition: b creato con malloc

void Produci(buff* b, msg mx){
	iniziaProduzione(b);	
	b->buffer = mx;
	printf("Messaggio prodotto: %d\n", b->buffer);
	fineProduzione(b);
}

void Consuma(buff* b){
	iniziaConsumo(b);
	printf("Messaggio consumato: %d\n", b->buffer);
	fineConsumo(b);
}

void iniziaConsumo(buff* b){
	pthread_mutex_lock(&(b->mutex));
	while(b->buffer_vuoto == 1){
		pthread_cond_wait(&(b->cons), &(b->mutex));
	}
}

void fineConsumo(buff* b){
	b->buffer_pieno = 0;
	b->buffer_vuoto = 1;
	pthread_cond_signal(&(b->prod));
	pthread_mutex_unlock(&(b->mutex));
}

void iniziaProduzione(buff* b){
	pthread_mutex_lock(&(b->mutex));
	while(b->buffer_pieno == 1){
		pthread_cond_wait(&(b->prod), &(b->mutex));
	}
}

void fineProduzione(buff* b){
	b->buffer_pieno = 1;
	b->buffer_vuoto = 0;
	pthread_cond_signal(&(b->cons));
	pthread_mutex_unlock(&(b->mutex));
}

void init_buffer(buff* b){
	b->buffer = 0;
	b->buffer_pieno = 0;
	b->buffer_vuoto = 1;
	pthread_mutex_init(&(b->mutex), NULL);
	pthread_cond_init(&(b->prod), NULL);
	pthread_cond_init(&(b->cons), NULL);
}

void free_buffer(buff* b){
	pthread_mutex_destroy(&(b->mutex));
	pthread_cond_destroy(&(b->prod));
	pthread_cond_destroy(&(b->cons));
	free(b);
}
