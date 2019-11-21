#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>

#include "header.h"
#include "procedure.h"

int main(){
	int pid, st;
	int id_shm = shmget(IPC_PRIVATE, sizeof(PriorityProdCons), IPC_CREAT|0664);
	if(id_shm < 0){perror("Errore shmget\n"); _exit(1);}
	PriorityProdCons* p = shmat(id_shm, 0, 0);
	inizializza_prod_cons(p);
	
	pid = fork();
	if(pid == 0){
		srand(time(NULL)*getpid());
		for(int i = 0; i < 3; i++){
			produci_alta_prio(p);
			sleep(2);
		}
		_exit(0);
	}
	for(int i = 0; i < 3; i++){
		pid = fork();
		if(pid == 0){
			srand(time(NULL)*getpid());
			for(i = 0; i < 3; i++){
				produci_bassa_prio(p);
				sleep(1);
			}
			_exit(0);
		}
	}
	pid = fork();
	if(pid == 0){
		for(int i = 0; i < 12; i++){
			consuma(p);
			sleep(1);
		}
		_exit(0);
	}
	
	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}

	rimuovi_prod_cons(p);
	shmctl(id_shm, IPC_RMID, 0);
return 0;
}
