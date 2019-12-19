#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "test.h"

#define DIM 5

int main(){
	pthread_t foo[5];
	pthread_t bar;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	Stack* s = (Stack*) malloc(sizeof(Stack));
	s->init = 0;
	StackInit(s, DIM);

	pthread_create(&bar, &attr, BarPop, (void*) s);

	for(int i=0; i<5; i++){
		pthread_create(&(foo[i]), &attr, FooPush, (void*) s);
	}

	for(int i=0; i<5; i++){
		pthread_join(foo[i], NULL);
		printf("Il thread %d che fa push ha terminato\n", i);
	}

	pthread_join(bar, NULL);
	printf("Il thread che fa pop ha terminato\n");

	StackRemove(s);
	free(s);
	return 0;
}
