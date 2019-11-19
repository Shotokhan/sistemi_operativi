#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "header.h"
#include "procedure.h"

#define N_CONS 10
#define N_PROD 10

#define NUM_ITERATIONS 15

#define NUM_COND 2

int main(){
	int pid, st;

	monitor M;
	init_monitor(&M, NUM_COND);	

	int ds_shm = shmget(IPC_PRIVATE, sizeof(magazzino), IPC_CREAT|IPC_EXCL|0664);
	if(ds_shm == -1){_exit(1);}
	magazzino* id_m = shmat(ds_shm, 0, 0);
	initMagazzino(id_m);

	for(int i = 0; i < N_CONS; i++){
		pid = fork();
		if(pid == 0){
			int j = 0;
			while(j < NUM_ITERATIONS){
				Cliente(&M, id_m);
				sleep(1);
				j++;
			}
			_exit(0);
		}		
	}

	for(int i = 0; i < N_PROD; i++){
		pid = fork();
		if(pid == 0){
			int j = 0;
			while(j < NUM_ITERATIONS){
				Fornitore(&M, id_m);
				sleep(1);
				j++;
			}
			_exit(0);
		}		
	}	
	
	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, con stato %d\n", WTERMSIG(st));
		}
	}

	shmctl(ds_shm, IPC_RMID, 0);	
	delete_monitor(&M);

return 0;
}
