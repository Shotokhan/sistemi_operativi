// bisogna usare l'area heap per condividere la memoria
// dato che i thread fanno parte dello stesso processo
// quindi: malloc

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "header.h"

int main(){
	pthread_t prodT[N_PROD];
	pthread_t consT[N_CONS];
	buff* b = (buff *) malloc(sizeof(buff));

	init_buffer(b);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	srand(time(NULL));

	for(int i = 0; i < N_PROD; i++){
		pthread_create(&(prodT[i]), &attr, Produttore, (void*) b);
	}
	
	for(int i = 0; i < N_CONS; i++){
		pthread_create(&(consT[i]), &attr, Consumatore, (void*) b);
	}

	int j = 0;
	int status;
	for(int i = 0; i < N_PROD + N_CONS; i++){
		if( i >= N_PROD ){
			pthread_join(consT[j], (void **) &status);
			printf("Il thread %d ha terminato con stato %d\n", consT[j], status);
			j+=1;	
		} else{
			pthread_join(prodT[i], (void **) &status);
			printf("Il thread %d ha terminato con stato %d\n", prodT[i], status);
		}
	}	

	free_buffer(b);

	pthread_exit(NULL);
}
