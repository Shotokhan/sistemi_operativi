#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "procedure.h"

#define N 5

int main(){
	pthread_t thread[N];
	int id[N] = {1, 2, 3, 4, 5};
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	g = (GestioneVoli*) malloc(sizeof(GestioneVoli));
	init_GestioneVoli(g);
	
	// Passo id per parametri ed il monitor mediante heap, con una variabile globale

	for(int i=0; i<N; i++){
		pthread_create(&(thread[i]), &attr, ThreadLifeline, (void*) &id[i]);
	}
	int st;
	for(int i=0; i<N; i++){
		pthread_join(thread[i], (void**) &st);
		printf("Il thread %d ha terminato con stato %d\n", thread[i], st);
	}
	
	if(g != NULL)
		remove_GestioneVoli(g);
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}
