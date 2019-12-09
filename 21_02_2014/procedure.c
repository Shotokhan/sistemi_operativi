#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

void inizializza(monitor_treno* m){
	m->stazione = 0;
	int r;
	r = pthread_mutex_init(&(m->mutex), 0);
	check_error(r, "Errore creazione mutex\n");
	r = pthread_cond_init(&(m->capotreno), 0);
	check_error(r, "Errore creazione condsem capotreno\n");
	r = pthread_cond_init(&(m->viaggiatore), 0);
	check_error(r, "Errore creazione condsem viaggiatore\n");	
	m->num_viaggiatori = 0;
	m->num_capitreno = 0;
	m->num_capitreno_wait = 0;
}

void rimuovi(monitor_treno* m){
	int r;
	r = pthread_mutex_destroy(&(m->mutex));
	check_error(r, "Errore distruzione mutex\n");
	r = pthread_cond_destroy(&(m->capotreno));
	check_error(r, "Errore distruzione condsem capotreno\n");
	r = pthread_cond_destroy(&(m->viaggiatore));
	check_error(r, "Errore distruzione condsem viaggiatore\n");		
}

int leggi_stazione(monitor_treno* m){
	inizio_lettura(m);
	int stazione = m->stazione;
	fine_lettura(m);
	return stazione;
}

void scrivi_stazione(monitor_treno* m, int stazione){
	inizio_scrittura(m);
	m->stazione = stazione;
	fine_scrittura(m);
}

void inizio_lettura(monitor_treno* m){
	pthread_mutex_lock(&(m->mutex));
	while(m->num_capitreno > 0){
		pthread_cond_wait(&(m->viaggiatore), &(m->mutex));
	}
	m->num_viaggiatori += 1;
	pthread_mutex_unlock(&(m->mutex));
}

void fine_lettura(monitor_treno* m){
	pthread_mutex_lock(&(m->mutex));
	m->num_viaggiatori -= 1;
	if(m->num_viaggiatori == 0){
		pthread_cond_signal(&(m->capotreno));
	}
	pthread_mutex_unlock(&(m->mutex));
}

void inizio_scrittura(monitor_treno* m){
	pthread_mutex_lock(&(m->mutex));
	while(m->num_capitreno > 0 || m->num_viaggiatori > 0){
		m->num_capitreno_wait += 1;
		pthread_cond_wait(&(m->capotreno), &(m->mutex));
		m->num_capitreno_wait -= 1;
	}
	m->num_capitreno += 1;
	pthread_mutex_unlock(&(m->mutex));
}

void fine_scrittura(monitor_treno* m){
	pthread_mutex_lock(&(m->mutex));
	m->num_capitreno -= 1;
	if(m->num_capitreno_wait > 0){
		pthread_cond_signal(&(m->capotreno));
	} else if(m->num_viaggiatori > 0){
		pthread_cond_broadcast(&(m->viaggiatore));
	}
	pthread_mutex_unlock(&(m->mutex));
}

void* capotreno(void* data){
	capotreno_data* buffer = (capotreno_data*) data;
	for(int i=0; i<10; i++){
		scrivi_stazione(buffer->m, i+1);
		printf("Sono il capotreno %d, ho scritto che il treno %d si trova alla stazione %d\n", buffer->capotreno_id, buffer->treno, i+1);
		sleep(3);
	}
	pthread_exit(0);
}

void* viaggiatore(void* data){
	viaggiatore_data* buffer = (viaggiatore_data*) data;
	int scelta = rand() % N_TRENI;
	monitor_treno* m = buffer->m[scelta];
	printf("Sono il viaggiatore %d, ho scelto il treno %d\n", buffer->viaggiatore_id, scelta);
	int stazione;
	for(int i=0; i<3; i++){
		stazione = leggi_stazione(m);
		printf("Sono il viaggiatore %d, ho letto che il treno %d si trova alla stazione %d\n", buffer->viaggiatore_id, scelta, stazione);
		sleep(rand() % 6 + 1);
	}
	pthread_exit(0);
}

void check_error(int retval, char* msg){
	if(retval < 0){
		perror(msg);
		pthread_exit(0);
	}
}
