#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>

#include "procedure.h"

#define numReq 4
#define numElab 2

int main(){
	int st;
	MonitorElaborazioni* m = (MonitorElaborazioni*) malloc(sizeof(MonitorElaborazioni));
	init_monitorElab(m);

	srand(time(NULL));

	pthread_t req[numReq];
	pthread_t elab[numElab];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(int i = 0; i < numReq + numElab; i++){
		if(i < numReq){
			pthread_create(&req[i], &attr, Richiedente, (void*) m);
		} else{
			pthread_create(&(elab[i - numReq]), &attr, Elaboratore, (void*) m);
		}
	}

	for(int i = 0; i < numReq + numElab; i++){
		if(i < numReq){
			pthread_join(req[i], (void**) &st);
			printf("Il thread richiedente %d ha terminato con stato %d\n", req[i], st);
		} else{
			pthread_join(elab[i - numReq], (void**) &st);
			printf("Il thread elaboratore %d ha terminato con stato %d\n", elab[i - numReq], st);
		}
	}

	pthread_attr_destroy(&attr);
	if(m != NULL)
		destroy_monitorElab(m);
	pthread_exit(NULL);
}
