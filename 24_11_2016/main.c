#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

int main(){
	pthread_t prod[4];
	pthread_t cons;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	MonitorCoda* m = (MonitorCoda*) malloc(sizeof(MonitorCoda));

	init_monitor(m);
	srand(time(NULL)*getpid());

	pthread_create(&cons, &attr, consumatore, (void*) m);

	for(int i=0; i<4; i++){
		pthread_create(&(prod[i]), &attr, produttore, (void*) m);
	}

	pthread_join(cons, NULL);
	printf("Il consumatore ha terminato\n");
	for(int i=0; i<4; i++){
		pthread_join(prod[i], NULL);
		printf("Il produttore %d ha terminato\n", i+1);
	}

	remove_monitor(m);
	free(m);
	pthread_exit(0);
}
