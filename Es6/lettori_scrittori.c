#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "header.h"
#include "waitProc.h"

#define N 5

int main(){
	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	if(id_shm < 0){
		perror("Errore shmget"); exit(1);
	}
	Buffer* p = (Buffer*)shmat(id_shm, NULL, 0);
	if(p==(void*)-1){
		perror("errore shmat");
	}	
	p->numLettori = 0;
	p->messaggio = 0;
	p->numScrittori = 0;

	int key_sem = IPC_PRIVATE;
	int id_sem = semget(key_sem, 4, IPC_CREAT|IPC_EXCL|0664);
	
	semctl(id_sem, MUTEXL, SETVAL, 1);
	semctl(id_sem, SYNCH, SETVAL, 1);
	semctl(id_sem, MUTEXS, SETVAL, 1);
	semctl(id_sem, MUTEX, SETVAL, 1);
	
	struct childrenType* childrenGroup = (struct childrenType*) malloc(2 * sizeof(struct childrenType));
	int pidArr1[N];
	int pidArr2[N];
	int* pidArr[2];
	pidArr[0] = pidArr1;
	pidArr[1] = pidArr2;
	char name1[8] = "lettore";
	char name2[10] = "scrittore";
	childrenGroup[0].name = name1;
	childrenGroup[1].name = name2;
	childrenGroup[0].pidArray = pidArr[0];
	childrenGroup[1].pidArray = pidArr[1];
	childrenGroup[0].numChildren = N;
	childrenGroup[1].numChildren = N;
	
	int pid, st, i=0;
	int ind[2] = {0, 0};
	while(i < 2*N){
		pid = fork();
		if(pid == 0){
			if(i%2 == 0){
				printf("Sono un lettore\n");
				Lettore(id_sem, p);
				_exit(0);
			}
				printf("Sono uno scrittore\n");
				Scrittore(id_sem, p);
				_exit(0);
		}
		pidArr[i%2][ind[i%2]] = pid;
		ind[i%2] += 1;
		i++;
	}
	
	waitProcesses(childrenGroup, 2, 2*N);
	semctl(key_sem, IPC_RMID, 0);
	shmctl(id_shm, IPC_RMID, 0);
return 0;
}
