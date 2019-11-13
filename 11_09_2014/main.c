#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <stdlib.h>

#include "header.h"

int main(){
	int pid, st;

	int key_shm = IPC_PRIVATE;
	int ds_shm = shmget(key_shm, sizeof(buffer), IPC_CREAT|IPC_EXCL|0664);
	if(ds_shm < 0){perror("Errore shmget\n"); _exit(1);}
	buffer* buf_ptr = shmat(ds_shm, NULL, 0);
	if(buf_ptr == (void*) -1){perror("Errore shmat\n"); _exit(1);}

	for(int i = 0; i < DIM; i++){
		buf_ptr->chiave[i] = 0;
		buf_ptr->valore[i] = 0;
		buf_ptr->stato[i] = VUOTO;
	}

	int key_sem = IPC_PRIVATE;
	int ds_sem = semget(key_sem, 4, IPC_CREAT|IPC_EXCL|0664);
	if(ds_sem < 0){perror("Errore semget\n"); _exit(1);}
	
	semctl(ds_sem, MSG_DSP, SETVAL, 0);
	semctl(ds_sem, SPAZIO_DSP, SETVAL, DIM);
	semctl(ds_sem, MUTEX_C, SETVAL, 1);
	semctl(ds_sem, MUTEX_P, SETVAL, 1);

	for(int i = 0; i < N_PROD; i++){
		pid = fork();
		if(pid == 0){
			srand(time(NULL)*getpid());
			int chiave = i + 1; // ho dichiarato questa variabile per una questione di leggibilita' del codice
			printf("Sono il produttore con pid %d e chiave %d\n", getpid(), chiave);
			Produttore(ds_sem, buf_ptr, chiave);
			_exit(0);
		}
	}

	for(int i = 0; i < N_CONS; i++){
		pid = fork();
		if(pid == 0){
			srand(time(NULL)*getpid());
			int chiave = i + 1;
			printf("Sono il consumatore con pid %d e chiave %d\n", getpid(), chiave);
			Consumatore(ds_sem, buf_ptr, chiave);
			_exit(0);
		}
	}

	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato l'esecuzione ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}
	
	shmctl(ds_sem, IPC_RMID, 0);
	shmctl(ds_sem, IPC_RMID, 0);
	return 0;
}
