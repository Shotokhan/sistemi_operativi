#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

#define N_CLIENT 3
#define N_SERVER 3

int main(){
	int shmid = shmget(ftok(KEY_PATH, KEY_CHAR_RISORSA), sizeof(Risorsa), IPC_CREAT|IPC_EXCL|0664);
	checkError(shmid, "Main - Errore creazione shared memory\n");
	Risorsa* r = (Risorsa*) shmat(shmid, NULL, 0);
	if(r == (void*) -1){
		checkError(-1, "Main - Errore shmat\n");
	}
	initMonitor(&(r->m));
	r->a = 0;
	r->b = 0;

	int qid = msgget(ftok(KEY_PATH, KEY_CHAR_QUEUE), IPC_CREAT|IPC_EXCL|0664);
	checkError(qid, "Main - Errore creazione coda\n");

	int pid, st;
	char* client[] = {"./client", NULL};
	char* server[] = {"./server", NULL};

	for(int i=0; i<N_SERVER; i++){
		pid = fork();
		if(pid == 0){
			execvp(server[0], server);
			perror("Errore exec\n");
		} else{
			checkError(pid, "Errore creazione processo\n");
		}
	}

	for(int i=0; i<N_CLIENT; i++){
		pid = fork();
		if(pid == 0){
			execvp(client[0], client);
			perror("Errore exec\n");
		} else{
			checkError(pid, "Errore creazione processo\n");
		}
	}

	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}
	
	removeMonitor(&(r->m));
	int R = shmctl(shmid, IPC_RMID, 0);
	checkError(R, "Errore rimozione risorsa condivisa\n");
	R = msgctl(qid, IPC_RMID, 0);
	checkError(R, "Errore rimozione coda\n");

	return 0;
}
