#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <pthread.h>

#include "header.h"

#define N_STAMPE 6

void* cliente(void* monitor){
	Monitor* m = (Monitor*) monitor;
	int copia;
	for(int i=1; i<=N_FILM; i++){
		copia = affitta(m, i);
		printf("Cliente - Ho affittato la copia %d del film %d\n", copia, i);
		sleep(1);
		restituisci(m, i, copia);
		printf("Cliente - Ho restituito la copia %d del film %d\n", copia, i);
	}
	pthread_exit(0);
}

void* controllo(void* monitor){
	Monitor* m = (Monitor*) monitor;
	for(int i=0; i<N_STAMPE; i++){
		stampa(m);
		sleep(1);
	}
	pthread_exit(0);
}

int affitta(Monitor* m, int id_film){
	pthread_mutex_lock(&(m->mutex));
	while(m->numCopie[id_film - 1] == 0){
		printf("Cliente - attendo il film %d\n", id_film);
		pthread_cond_wait(&(m->affitto[id_film - 1]), &(m->mutex));
		printf("Cliente - sono stato sbloccato sul film %d\n", id_film);
	}
	int i = ricercaDVD(id_film, 0, m->dvd, N_FILM * N_COPIE);
	if(i < 0){
		pthread_mutex_unlock(&(m->mutex));
		check_error(i, "Errore di sincronizzazione oppure di passaggio parametri\n");
	}
	m->dvd[i].stato = AFFITTATO;
	m->numCopie[id_film - 1] -= 1;
	pthread_mutex_unlock(&(m->mutex));
	return m->dvd[i].id_copia;
}

void restituisci(Monitor* m, int id_film, int id_copia){
	// posso fare questa ricerca non in mutex perché non controlla variabili
	// che vengono modificate in concorrenza
	int i = ricercaDVD(id_film, id_copia, m->dvd, N_FILM * N_COPIE);
	check_error(i, "Errore passaggio parametri\n");
	pthread_mutex_lock(&(m->mutex));
	m->dvd[i].stato = DISPONIBILE;
	m->numCopie[id_film - 1] += 1;
	pthread_cond_signal(&(m->affitto[id_film - 1]));
	pthread_mutex_unlock(&(m->mutex));
}

void stampa(Monitor* m){
	char* stato[] = {"DISPONIBILE", "AFFITTATO"};
	// faccio la stampa in mutua esclusione sia per evitare interleaving sia
	// perché altrimenti non sarebbe una vera istantanea del sistema
	pthread_mutex_lock(&(m->mutex));
	printf("Stampo lo stato dei DVD del negozio\n");
	for(int i=0; i < N_FILM * N_COPIE; i++){
		printf("Film %d, copia %d, stato: %s\n", m->dvd[i].id_film, m->dvd[i].id_copia, stato[m->dvd[i].stato]);
	}
	pthread_mutex_unlock(&(m->mutex));
}

void init_monitor(Monitor* m){
	int r = pthread_mutex_init(&(m->mutex), 0);
	check_error(r, "Errore creazione mutex\n");
	for(int i=0; i<N_FILM; i++){
		r = pthread_cond_init(&(m->affitto[i]), 0);
		check_error(r, "Errore creazione cond\n");
	}
	for(int i=0; i<N_FILM; i++){
		for(int j=0; j<N_COPIE; j++){
			m->dvd[i + j*N_FILM].id_film = i + 1;
			m->dvd[i + j*N_FILM].id_copia = j + 1;
			m->dvd[i + j*N_FILM].stato = DISPONIBILE;
		}
		m->numCopie[i] = N_COPIE;
	}
}

void remove_monitor(Monitor* m){
	int r = pthread_mutex_destroy(&(m->mutex));
	check_error(r, "Errore distruzione mutex\n");
	for(int i=0; i<N_FILM; i++){
		r = pthread_cond_destroy(&(m->affitto[i]));
		check_error(r, "Errore distruzione cond\n");
	}
}

void check_error(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		pthread_exit(0);
	}
}

int ricercaDVD(int id_film, int id_copia, DVD* vettore, int dim){
	if(id_copia > 0){
		for(int i=0; i<dim; i++){
			if(vettore[i].id_film == id_film && vettore[i].id_copia == id_copia){
				return i;
			}
		}
		return -1;
	} else{
		for(int i=0; i<dim; i++){
			if(vettore[i].id_film == id_film && vettore[i].stato == DISPONIBILE){
				return i;
			}
		}
		return -1;
	}
}
