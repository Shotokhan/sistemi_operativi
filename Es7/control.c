#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

#define M 10

int main(){
	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(buffer), IPC_CREAT|IPC_EXCL|0666);
	if(id_shm < 0){
		perror("Errore shmget"); exit(1);
	}
	buffer* p = (buffer*)shmat(id_shm, NULL, 0);
	if(p==(void*)-1){
		perror("errore shmat");
	}
	char str[8] = "default";	
	strcpy(p->prox_appello, str);
	p->num_prenotati = 0;
	p->num_lettori = 0;

	int key_sem = ftok(".", 'C');
	int id_sem = semget(key_sem, 3, IPC_CREAT|IPC_EXCL|0666);
	
	semctl(id_sem, SYNCH, SETVAL, 1);
	semctl(id_sem, MUTEX_P, SETVAL, 1);
	semctl(id_sem, MUTEX_L, SETVAL, 1);	
	
	char* args1[] = {"./docente", NULL};
	char* args2[] = {"./studente", NULL};

	int pid, st;
	
	pid = fork();
	if(pid == 0){
		execvp(args1[0], args1);
		perror("Exec fallita\n"); _exit(1);
	}	
	
	for(int i = 0; i < M; i++){
		pid = fork();
		if(pid == 0){
			execvp(args2[0], args2);
			perror("Exec fallita\n"); _exit(1);
		}
	}

	while((pid=wait(&st)) > 0){
		printf("Il processo %d ha terminato ", pid);
		if (WIFEXITED(st)){
			printf("con stato: %d\n", WEXITSTATUS(st));
		} else {
			if (WIFSIGNALED(st)){
				printf(" con terminazione involontaria per segnale %d\n", WTERMSIG(st));
			}
		}
	}

	semctl(key_sem, IPC_RMID, 0);
	shmctl(id_shm, IPC_RMID, 0);
return 0;
}
