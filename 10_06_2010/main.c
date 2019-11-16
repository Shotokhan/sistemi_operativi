#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#include "header.h"
#include "procedure.h"

#define N_COND 2
#define N_WRITERS 10

int main(){
	int id_meteo = 0;
	meteo* ds_shm;
	ds_shm = init_meteo(&id_meteo);
	
	monitor M;
	init_monitor(&M, N_COND);

	int pid, st, pidWriter;

	// Scrittore
	pidWriter = fork();
	if(pidWriter == 0){
		int i = 0;
		srand(time(NULL)*getpid());
		while(i < 20){
			scrittore(&M, ds_shm);
			sleep(2);
			i++;
		}
		_exit(0);
	}
	// Lettori
	for(int i = 0; i < N_WRITERS; i++){
		pid = fork();
		if(pid == 0){
			int j = 0;
			srand(time(NULL)*getpid());
			while(j < 10){
				lettore(&M, ds_shm);
				sleep(1);
				j++;
			}
			_exit(0);
		}
	}
	// Attesa fine processi
	while((pid=wait(&st))>0){
		printf("Il processo ");
		if(pid == pidWriter){
			printf("scrittore ");
		} else{
			printf("lettore ");
		}
		printf("con pid %d ha terminato: ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}

	remove_monitor(&M);
	remove_meteo(id_meteo);
return 0;
}
