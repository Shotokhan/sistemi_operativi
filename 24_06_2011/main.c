#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "header.h"
#include "proc.h"

int main(){
	int pid;
	int shmid = shmget(ftok(KEY_PATH, KEY_CHAR_QUEUE), sizeof(codaCircolare), IPC_CREAT|IPC_EXCL|0664);
	checkError(shmid, "Errore creazione shared memory nel main\n");
	codaCircolare* q = (codaCircolare*) shmat(shmid, NULL, 0);
	if(q == (void*) -1){
		checkError(-1, "Errore shmat nel main\n");
	}
	initCoda(q);

	pid = fork();
	if(pid == 0){
		printf("Sono lo scheduler %d\n", getpid());
		Schedulatore(q);
		_exit(0);
	} else{
		checkError(pid, "Errore creazione processo\n");
	}

	for(int i=0; i<N_UTENTI; i++){
		pid = fork();
		if(pid == 0){
			Utente(q);
			printf("Sono l'utente %d\n", getpid());
			_exit(0);
		} else{
			checkError(pid, "Errore creazione processo\n");
		}
	}

	waitProcesses();

	removeCoda(q);
	shmctl(shmid, IPC_RMID, 0);
	return 0;
}
