#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <pthread.h>

#include "header.h"

#define N_CLIENT 4

int main(){
	pthread_t client[N_CLIENT];
	pthread_t control;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	Monitor* m = (Monitor*) malloc(sizeof(Monitor));
	init_monitor(m);

	pthread_create(&control, &attr, controllo, (void*) m);

	for(int i=0; i<N_CLIENT; i++){
		pthread_create(&(client[i]), &attr, cliente, (void*) m);
	}

	for(int i=0; i<N_CLIENT; i++){
		pthread_join(client[i], 0);
		printf("Un cliente ha terminato\n");
	}
	pthread_join(control, 0);
	printf("Il control ha terminato\n");

	remove_monitor(m);
	if(m != NULL){
		free(m);
	}
	pthread_exit(0);
}
