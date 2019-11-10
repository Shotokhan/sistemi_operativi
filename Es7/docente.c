#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "header.h"

#define N 3

int main(){

	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(buffer), IPC_CREAT);
	if(id_shm < 0){
		perror("Docente: Errore shmget"); _exit(1);
	}
	buffer* p = (buffer*)shmat(id_shm, NULL, 0);
	if(p==(void*)-1){
		perror("Docente: Errore shmat"); _exit(1);
	}
	int key_sem = ftok(".", 'C');
	int id_sem = semget(key_sem, 3, IPC_CREAT);
	
	unsigned int i = 0;
	
	char arr[3][11] = {"14-12-2019", "27-01-2020", "05-02-2020"};
	
	while(i < N){
		doc_aggiorna(id_sem, p, arr[i]);
		sleep(3);
		doc_leggi(id_sem, p, 1);
		i++;
	}
	return 0;
}
