#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>

#include "procedure.h"

int main(){
	srand(time(NULL)*getpid());
	key_t semkey = ftok("./", 'b');
	key_t shmkey = ftok("./", 'c');
	int shm_id = shmget(shmkey, sizeof(BufferCircolare), IPC_CREAT|IPC_EXCL|0664);
	if(shm_id < 0){
		perror("Errore shmget\n"); _exit(1);
	}
	int sem_id = semget(semkey, 3, IPC_CREAT|IPC_EXCL|0664);
	if(sem_id < 0){
		perror("Errore semget\n"); _exit(1);
	}
	BufferCircolare* buf = shmat(shm_id, 0, 0);

	initBuffer(buf);

	semctl(sem_id, MUTEX, SETVAL, 1);
	semctl(sem_id, PROD, SETVAL, N);
	semctl(sem_id, CONS, SETVAL, 0);
	
	char* prod[] = {"./produttore", NULL};
	char* cons[] = {"./consumatore", NULL};
		
	int pid, st;

	for(int i=0; i<10; i++){
		pid = fork();
		if(pid == 0){
			execvp(prod[0], prod);
			perror("Errore exec\n");
		}
		sleep(rand() % 3 + 1);
	}
	pid = fork();
	if(pid == 0){
		execvp(cons[0], cons);
		perror("Errore exec\n");
	}

	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}

	semctl(sem_id, 3, IPC_RMID);
	shmctl(shm_id, IPC_RMID, 0);
	
	_exit(0);
}
