#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

int main(){
	// inizio setup
	int r;
	pthread_t capitreno[N_TRENI];
	pthread_t viaggiatori[N_VIAGGIATORI];
	
	pthread_attr_t attr;
	r = pthread_attr_init(&attr);
	check_error(r * (-1), "Errore init attr\n");
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	monitor_treno** m = (monitor_treno**) malloc(N_TRENI*sizeof(monitor_treno*));

	capotreno_data** args1 = (capotreno_data**) malloc(N_TRENI*sizeof(capotreno_data*));

	viaggiatore_data** args2 = (viaggiatore_data**) malloc(N_VIAGGIATORI*sizeof(viaggiatore_data*));

	for(int i=0; i<N_TRENI; i++){
		m[i] = (monitor_treno*) malloc(sizeof(monitor_treno));
		args1[i] = (capotreno_data*) malloc(sizeof(capotreno_data));
		inizializza(m[i]);
		args1[i]->m = m[i];
		args1[i]->capotreno_id = i;
		args1[i]->treno = i;
	}

	for(int i=0; i<N_VIAGGIATORI; i++){
		args2[i] = (viaggiatore_data*) malloc(sizeof(viaggiatore_data));
		for(int j=0; j<N_TRENI; j++){
			args2[i]->m[j] = m[j];
		}
		args2[i]->viaggiatore_id = i;
	}
	// fine setup
	
	// inizio creazione thread
	for(int i=0; i<N_TRENI; i++){
		r = pthread_create(&capitreno[i], &attr, capotreno, (void*) args1[i]);
		check_error(r * (-1), "Errore creazione thread capotreno\n");
	}

	srand(time(NULL));
	for(int i=0; i<N_VIAGGIATORI; i++){
		r = pthread_create(&viaggiatori[i], &attr, viaggiatore, (void*) args2[i]);
		check_error(r * (-1), "Errore creazione thread viaggiatore\n");
	}
	// fine creazione thread

	// inizio wait e deallocazione
	for(int i=0; i<N_TRENI; i++){
		r = pthread_join(capitreno[i], 0);
		check_error(r * (-1), "Errore join\n");
		printf("Il thread capotreno %d con id %d ha terminato\n", args1[i]->capotreno_id, capitreno[i]);
		if(args1[i] != NULL){
			free(args1[i]);
		}
	}
	
	for(int i=0; i<N_VIAGGIATORI; i++){
		r = pthread_join(viaggiatori[i], 0);
		check_error(r * (-1), "Errore join\n");
		printf("Il thread viaggiatore %d con id %d ha terminato\n", args2[i]->viaggiatore_id, viaggiatori[i]);
		if(args2[i] != NULL){
			free(args2[i]);
		}
	}

	for(int i=0; i<N_TRENI; i++){
		rimuovi(m[i]);
		if(m[i] != NULL){
			free(m[i]);
		}
	}

	if(m != NULL){
		free(m);
	}
	if(args1 != NULL){
		free(args1);
	}
	if(args2 != NULL){
		free(args2);
	}
	r = pthread_attr_destroy(&attr);
	check_error(r * (-1), "Errore destroy attr\n");
	// fine wait e deallocazione

	pthread_exit(0);
}
