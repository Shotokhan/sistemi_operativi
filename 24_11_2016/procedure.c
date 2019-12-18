#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

#include "header.h"

int produzione(MonitorCoda* m, int valore){
	pthread_mutex_lock(&(m->mutex));
	while(m->num_occupati == DIM){
		if(m->produttori_in_attesa == 3){
			pthread_cond_signal(&(m->prod));
			pthread_mutex_unlock(&(m->mutex));
			return 1;
		}
		m->produttori_in_attesa += 1;
		pthread_cond_wait(&(m->prod), &(m->mutex));
		m->produttori_in_attesa -= 1;
	}
	m->vettore[m->coda] = valore;
	m->coda = (m->coda + 1) % DIM;
	m->num_occupati += 1;
	printf("Produttore - ho prodotto %d\n", valore);
	pthread_cond_signal(&(m->cons));
	pthread_mutex_unlock(&(m->mutex));
	return 0;
}

int consumazione(MonitorCoda* m){
	pthread_mutex_lock(&(m->mutex));
	while(m->num_occupati == 0){
		pthread_cond_wait(&(m->cons), &(m->mutex));
	}
	int valore = m->vettore[m->testa];
	m->testa = (m->testa + 1) % DIM;
	m->num_occupati -= 1;
	pthread_cond_signal(&(m->prod));
	pthread_mutex_unlock(&(m->mutex));
	return valore;
}

void init_monitor(MonitorCoda* m){
	int r;
	r = pthread_mutex_init(&(m->mutex), NULL);
	check_error(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(m->prod), NULL);
	check_error(r, "Errore creazione prod\n");
	r = pthread_cond_init(&(m->cons), NULL);
	check_error(r, "Errore creazione prod\n");
	for(int i=0; i<DIM; i++){
		m->vettore[i] = 0;
	}
	m->testa = 0;
	m->coda = 0;
	m->produttori_in_attesa = 0;
	m->num_occupati = 0;
}

void remove_monitor(MonitorCoda* m){
	int r;
	r = pthread_mutex_destroy(&(m->mutex));
	check_error(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(m->prod));
	check_error(r, "Errore distruzione cond\n");
	r = pthread_cond_destroy(&(m->cons));
	check_error(r, "Errore distruzione cond\n");
}

void check_error(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		pthread_exit(0);
	}
}

void* produttore(void* monitor){
	MonitorCoda* m = (MonitorCoda*) monitor;
	int valore, ret;
	for(int i=0; i<4; i++){
		valore = rand() % 100;
		while(1){
			ret = produzione(m, valore);
			if(ret == 0){ break; }
			printf("Produttore - ho trovato 3 altri produttori in attesa\n");
			sleep(3);
		}
		sleep(1);
	}
	pthread_exit(0);
}

void* consumatore(void* monitor){
	MonitorCoda* m = (MonitorCoda*) monitor;
	int valore;
	for(int i=0; i<16; i++){
		valore = consumazione(m);
		printf("Consumatore - ho consumato %d\n", valore);
		sleep(2);
	}
	pthread_exit(0);
}
