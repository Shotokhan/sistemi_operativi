#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

#define N_PROD 4
#define N_CONS 2

int main(){
	int r;
	pthread_t prod[N_PROD];
	pthread_t cons[N_CONS];
	
	pthread_attr_t attr;
	r = pthread_attr_init(&attr);
	checkError(r, "Errore attr init\n");
	r = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	checkError(r, "Errore attr set_detach_state\n");
	
	GestioneIO* g = (GestioneIO*) malloc(sizeof(GestioneIO));
	Data** D = (Data**) malloc(sizeof(Data)*(N_PROD + N_CONS));
	for(int i=0; i<N_PROD+N_CONS; i++){
		D[i] = (Data*) malloc(sizeof(Data));
		D[i]->g = g;
		D[i]->threadId = (i < N_PROD ? i : i - N_PROD);
	}
	Inizializza(g);	
	srand(time(NULL));

	for(int i=0; i<N_PROD; i++){
		r = pthread_create(&(prod[i]), &attr, Produttore, (void*) D[i]);
		checkError(r, "Errore pthread create\n");
	}
	
	for(int i=N_PROD; i<N_PROD+N_CONS; i++){
		r = pthread_create(&(cons[i-N_PROD]), &attr, Consumatore, (void*) D[i]);
		checkError(r, "Errore pthread create\n");
	}

	for(int i=0; i<N_PROD+N_CONS; i++){
		pthread_t t = (i < N_PROD ? prod[i] : cons[i - N_PROD]);
		char* t_name[2] = {"produttore", "consumatore"};
		pthread_join(t, 0);
		printf("Il thread %s %d ha terminato\n", t_name[i >= N_PROD], D[i]->threadId);
		free(D[i]);
	}
	free(D);
	Rimuovi(g);
	if(g != NULL){
		free(g);
	}
	r = pthread_attr_destroy(&attr);
	checkError(r, "Errore attr destroy\n");

	pthread_exit(0);
}
