#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

int main(){
	pthread_t threads[5];
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	MonitorOperandi* op = (MonitorOperandi*) malloc(sizeof(MonitorOperandi));
	MonitorRisultati* r = (MonitorRisultati*) malloc(sizeof(MonitorRisultati));

	inizializza(op, r);
	srand(time(NULL)*getpid());	

	dataOpRis** Data = (dataOpRis**) malloc(sizeof(dataOpRis*)*3);
	for(int i=0; i<3; i++){
		Data[i] = (dataOpRis*) malloc(sizeof(dataOpRis));
		(Data[i])->op = op;
		(Data[i])->r = r;
		(Data[i])->thread_id = i;
	}

	pthread_create(&(threads[0]), &attr, Prod, (void*) op);
	for(int i=0; i<3; i++){
		pthread_create(&(threads[i+1]), &attr, Cons_Prod, (void*) Data[i]);
	}
	pthread_create(&(threads[4]), &attr, Cons, (void*) r);

	for(int i=0; i<5; i++){
		pthread_join(threads[i], 0);
		switch(i){
			case 0:
				printf("Il thread che produce gli operandi ha terminato\n");
				break;
			case 4:
				printf("Il thread che preleva i risultati ha terminato\n");
				break;
			default:
				printf("Il thread %d che preleva gli operandi e produce i risultati ha terminato\n", (Data[i-1])->thread_id);
				free(Data[i-1]);
				break;
		}
	}
	free(Data);
	rimuovi(op, r);
	if(op != NULL){
		free(op);
	}
	if(r != NULL){
		free(r);
	}
	pthread_exit(0);
}
