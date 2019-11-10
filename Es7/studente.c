#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"

int main(){

	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(buffer), IPC_CREAT);
	if(id_shm < 0){
		perror("Studente: Errore shmget"); _exit(1);
	}
	buffer* p = (buffer*)shmat(id_shm, NULL, 0);
	if(p==(void*)-1){
		perror("Studente: Errore shmat"); _exit(1);
	}
	int key_sem = ftok(".", 'C');
	int id_sem = semget(key_sem, 3, IPC_CREAT);

	srand(time(NULL)*getpid());
	int seconds = rand() % 9;
	
	sleep(seconds);
	stud_leggi(id_sem, p);
	stud_prenota(id_sem, p);
	
return 0;
}
