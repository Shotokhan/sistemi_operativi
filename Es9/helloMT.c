#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define N_THREADS 5

void *PrintHello(void *threadid){
	int* t = (int*) threadid;
	printf("%d: Hello world\n", *t);
	pthread_exit((void*) 0);
}

int main(){
	int k=0;
	pthread_t thread[N_THREADS];
	for(; k < N_THREADS; k++){
		pthread_create(&thread[k], NULL, PrintHello, (void*) &k);
	}
	pthread_exit((void*) 0);
	return 0;
}
