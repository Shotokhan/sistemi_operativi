#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "procedure.h"

void* ThreadLifeline(void* heap){
	int* id_ptr = (int*) heap;
	int id = *id_ptr;
	printf("Sono il thread a cui è stato passato l'id %d\n", id);
	int quota[3] = {1000, 2000, 1000};
	InserisciVolo(g, id);
	// Ho gestito il caso in cui la trylock non entra
	// mediante un retry qui con una sleep random
	int r = 0;
	for(int i = 0; i < 3; i++){
		do {
			r = AggiornaVolo(g, id, quota[i]);
			if (r == -1){
				sleep(rand() % 3 + 1);
			}
		} while(r == -1);
	}
	do{
		r = RimuoviVolo(g, id);
		if (r == -1){
			sleep(1);
		}
	} while(r == -1);
	pthread_exit(NULL);
}

void InserisciVolo (GestioneVoli* g, int id){
	pthread_mutex_lock(&(g->mutex));
	while(g->num_liberi == 0){
		pthread_cond_wait(&(g->prod), &(g->mutex));
	}
	int i = 0;
	while(i < DIM && g->stato[i] != LIBERO){
		i++;
	}
	g->stato[i] = IN_USO;
	g->num_liberi -= 1;
	pthread_mutex_unlock(&(g->mutex));

	(g->vettore_voli[i]).id = id;
	(g->vettore_voli[i]).quota = 0;
	sleep(1);
	printf("Inserimento del volo %d completato\n", id);	

	pthread_mutex_lock(&(g->mutex));
	g->stato[i] = OCCUPATO;
	pthread_mutex_unlock(&(g->mutex));
}

int AggiornaVolo (GestioneVoli* g, int id, int quota){
	if(pthread_mutex_trylock(&(g->mutex)) == 0){
		int i = 0;
		while(i < DIM && !(g->stato[i] == OCCUPATO && (g->vettore_voli[i]).id == id) ){
			i++;
		}
		g->stato[i] = IN_USO;
		pthread_mutex_unlock(&(g->mutex));

		(g->vettore_voli[i]).quota = quota;
		int sleepTime = rand() % 3 + 1;
		sleep(sleepTime);
		g->stato[i] = OCCUPATO;
		printf("Aggiornamento del volo %d alla quota %d completato \n", id, quota);
		return 0;
	} else{
		printf("Aggiornamento del volo %d alla quota %d non riuscito. Riprovare\n", id, quota);
		return -1;
	}
}

int RimuoviVolo (GestioneVoli* g, int id){
	if(pthread_mutex_trylock(&(g->mutex)) == 0){
		int i = 0;
		while(i < DIM && !(g->stato[i] == OCCUPATO && (g->vettore_voli[i]).id == id) ){
			i++;
		}
		g->stato[i] = IN_USO;
		pthread_mutex_unlock(&(g->mutex));

		sleep(1);

		if(pthread_mutex_trylock(&(g->mutex)) == 0){
			g->num_liberi += 1;	
			pthread_mutex_unlock(&(g->mutex));
			g->stato[i] = LIBERO;
			pthread_cond_signal(&(g->prod));
			printf("Rimozione del volo %d completata\n", id);
			return 0;
		} else{
			g->stato[i] = OCCUPATO;
			printf("Rimozione del volo %d non riuscita. Riprovare\n", id);
		}
	} else{
		printf("Rimozione del volo %d non riuscita. Riprovare\n", id);
	}
	return -1;
}

void init_GestioneVoli(GestioneVoli* g){
	int r = pthread_mutex_init(&(g->mutex), NULL);
	if(r!=0){perror("Creazione mutex non riuscita\n"); pthread_exit(NULL);}
	r = pthread_cond_init(&(g->prod), NULL);
	if(r!=0){perror("Creazione cond non riuscita\n"); pthread_exit(NULL);}
	g->num_liberi = DIM;
	for(int i = 0; i < DIM; i++){
		(g->vettore_voli[i]).id = 0;
		(g->vettore_voli[i]).quota = 0;
		g->stato[i] = LIBERO;
	}
}

void remove_GestioneVoli(GestioneVoli* g){
	pthread_mutex_destroy(&(g->mutex));
	pthread_cond_destroy(&(g->prod));
	free(g);
}
