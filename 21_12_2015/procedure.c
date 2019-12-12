#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

void Inizializza(GestioneIO* g){
	int r;
	r = pthread_mutex_init(&(g->mutex), 0);
	checkError(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(g->prod), 0);
	checkError(r, "Errore creazione condsem\n");
	g->numOccupati = 0;
	g->numLiberi = DIM;
	for(int i=0; i<DIM; i++){
		g->stato[i] = LIBERO;
		g->vettore[i].indirizzo = 0;
		g->vettore[i].dato = 0;
	}
}

void Produci(GestioneIO* g, Buffer* b){
	pthread_mutex_lock(&(g->mutex));
	while(g->numLiberi == 0){
		pthread_cond_wait(&(g->prod), &(g->mutex));
	}
	int i=0;
	while(i < DIM && g->stato[i] != LIBERO){
		i++;
	}
	if(i == DIM){
		checkError(-1, "Errore di sincronizzazione produttore\n");
	}
	g->stato[i] = IN_USO;
	g->numLiberi -= 1;
	pthread_mutex_unlock(&(g->mutex));

	g->vettore[i].indirizzo = b->indirizzo;
	g->vettore[i].dato = b->dato;

	pthread_mutex_lock(&(g->mutex));
	g->stato[i] = OCCUPATO;
	g->numOccupati += 1;
	pthread_mutex_unlock(&(g->mutex));
}

int Consuma(GestioneIO* g, Buffer* b){
	pthread_mutex_lock(&(g->mutex));
	if(g->numOccupati == 0){
		pthread_mutex_unlock(&(g->mutex));
		return 1;
	}
	int i=0;
	while(i < DIM && g->stato[i] != OCCUPATO){
		i++;
	}
	if(i == DIM){
		checkError(-1, "Errore di sincronizzazione consumatore\n");
	}
	g->stato[i] = IN_USO;
	g->numOccupati -= 1;
	pthread_mutex_unlock(&(g->mutex));

	b->indirizzo = g->vettore[i].indirizzo;
	b->dato = g->vettore[i].dato;

	pthread_mutex_lock(&(g->mutex));
	g->stato[i] = LIBERO;
	g->numLiberi += 1;
	pthread_cond_signal(&(g->prod));
	pthread_mutex_unlock(&(g->mutex));
	return 0;
}

void* Produttore(void* data){
	Data* dat = (Data*) data;
	Buffer b;
	b.indirizzo = rand() % 11;
	b.dato = rand() % 11;
	for(int i = 0; i < 3; i++){
		Produci(dat->g, &b);
		printf("Sono il produttore %d, ho prodotto indirizzo %d e dato %d\n", dat->threadId, b.indirizzo, b.dato);
		b.indirizzo += 1;
		b.dato += 1;
		sleep(1);
	}
	pthread_exit(0);
}

void* Consumatore(void* data){
	Data* dat = (Data*) data;
	int r = 0;
	Buffer b;
	for(int i = 0; i < 4; i++){
		int k = 0;
		while(r == 0){
			r = Consuma(dat->g, &b);
			if(r == 0){
				printf("Sono il consumatore %d, ho consumato indirizzo %d e dato %d\n", dat->threadId, b.indirizzo, b.dato);
				k++;
			}
		}
		printf("Sono il consumatore %d, ho consumato %d valori nell'iterazione %d\n", dat->threadId, k, i+1);
		sleep(3);
		r = 0;
	}
	pthread_exit(0);
}

void Rimuovi(GestioneIO* g){
	int r;
	r = pthread_mutex_destroy(&(g->mutex));
	checkError(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(g->prod));
	checkError(r, "Errore distruzione condsem\n");
}

void checkError(int retVal, char* msg){
	if(retVal != 0){
		perror(msg);
		pthread_exit(0);
	}
}
