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

#define n_g 1
#define n_a 1
#define n_d 3

int main(){
	pthread_t gen[n_g];
	pthread_t agg[n_a];
	pthread_t dest[n_d];

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	monitorVettore* v = (monitorVettore*) malloc(sizeof(monitorVettore));
	monitorBuffer* b = (monitorBuffer*) malloc(sizeof(monitorBuffer));

	initMonitorVettore(v);
	initMonitorBuffer(b);

	bufGeneratore* bG = (bufGeneratore*) malloc(sizeof(bufGeneratore)*n_g);
	bufAggiornatore* bA = (bufAggiornatore*) malloc(sizeof(bufAggiornatore)*n_a);
	bufDestinatario* bD = (bufDestinatario*) malloc(sizeof(bufDestinatario)*n_d);

	srand(time(NULL));

	for(int i = 0; i < n_g + n_a + n_d; i++){
		if(i < n_g){
			bG[i].v = v;
			bG[i].threadId = i;
			pthread_create(&gen[i], &attr, generatore, (void*) &bG[i]);
		} else if(i < n_g + n_a){
			int index = i - n_g;
			bA[index].v = v;
			bA[index].b = b;
			bA[index].threadId = i;
			pthread_create(&agg[index], &attr, aggiornatore, (void*) &bA[index]);
		} else{
			int index = i - n_a - n_g;
			bD[index].b = b;
			bD[index].threadId = i;
			pthread_create(&dest[index], &attr, destinatario, (void*) &bD[index]);
		}
	}

	for(int i = 0; i < n_g + n_a + n_d; i++){
		int st;
		if(i < n_g){
			pthread_join(gen[i], (void**) &st);
		} else if(i < n_g + n_a){
			int index = i - n_g;
			pthread_join(agg[index], (void**) &st);
		} else{
			int index = i - n_a - n_g;
			pthread_join(dest[index], (void**) &st);
		}
		printf("Il thread %d ha terminato con stato %d\n", i, st);
	}

	freeMonitorVettore(v);
	freeMonitorBuffer(b);
	free(v);
	free(b);
	free(bG);
	free(bA);
	free(bD);
	pthread_attr_destroy(&attr);

	pthread_exit(0);
}
