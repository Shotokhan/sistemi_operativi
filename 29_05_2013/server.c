#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <pthread.h>

#include "header.h"

int main(){
	pthread_t workers[N_WORKERS];
	pthread_t manager;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	buffer* b = (buffer*) malloc(sizeof(buffer));
	initBuffer(b);
	managerData* m = (managerData*) malloc(sizeof(managerData));
	m->b = b;
	
	for(int i=0; i<N_WORKERS; i++){
		pthread_create(&(workers[i]), NULL, Worker, (void*) b);
		m->workers[i] = workers[i];
	}
	pthread_create(&manager, &attr, Manager, (void*) m);

	pthread_join(manager, NULL);
	
	removeBuffer(b);
	free(b);
	free(m);
	pthread_attr_destroy(&attr);

	_exit(0);
}
