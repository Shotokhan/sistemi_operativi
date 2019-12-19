#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "header.h"

void initMonitor(monitorHoare* m, int num_var_cond){
	m->mutex = semget(ftok(KEY_PATH, KEY_CHAR_MUTEX), 1, IPC_CREAT|IPC_EXCL|0664);
	checkError(m->mutex, "Errore creazione mutex\n");
	semctl(m->mutex, 0, SETVAL, 1);
	m->n_cond = num_var_cond;
	m->shmid = shmget(ftok(KEY_PATH, KEY_CHAR_SHM), sizeof(int)*(m->n_cond), IPC_CREAT|IPC_EXCL|0664);
	checkError(m->shmid, "Errore creazione shared memory\n");
	m->cond_count = (int*) shmat(m->shmid, NULL, 0);
	if(m->cond_count == (void*) -1){
		checkError(-1, "Errore shmat\n");
	}
	m->condsem = semget(ftok(KEY_PATH, KEY_CHAR_COND), m->n_cond, IPC_CREAT|IPC_EXCL|0664);
	checkError(m->condsem, "Errore creazione condsem\n");
	for(int i=0; i < m->n_cond; i++){
		m->cond_count[i] = 0;
		semctl(m->condsem, i, SETVAL, 0);
	}
	m->urgent_sem = semget(ftok(KEY_PATH, KEY_CHAR_URGENT), 1, IPC_CREAT|IPC_EXCL|0664);
	checkError(m->urgent_sem, "Errore creazione urgent sem\n");
	semctl(m->urgent_sem, 0, SETVAL, 0);
	m->urgent_count = 0;
}

void removeMonitor(monitorHoare* m){
	semctl(m->mutex, 0, IPC_RMID);
	semctl(m->condsem, m->n_cond, IPC_RMID);
	semctl(m->urgent_sem, 0, IPC_RMID);
	shmctl(m->shmid, IPC_RMID, NULL);
	m->cond_count = NULL;
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		_exit(1);
	}
}

void enterMonitor(monitorHoare* m){
	Wait_Sem(m->mutex, 0);
}

void leaveMonitor(monitorHoare* m){
	if(m->urgent_count > 0){
		Signal_Sem(m->urgent_sem, 0);
	} else{
		Signal_Sem(m->mutex, 0);
	}
}

void waitCondition(monitorHoare* m, unsigned int cond){
	m->cond_count[cond] += 1;
	Signal_Sem(m->mutex, 0);
	Wait_Sem(m->condsem, cond);
	m->cond_count[cond] -= 1;
}

void signalCondition(monitorHoare* m, unsigned int cond){
	if(m->cond_count[cond] > 0){
		m->urgent_count += 1;
		Signal_Sem(m->condsem, cond);
		Wait_Sem(m->urgent_sem, 0);
		m->urgent_count -= 1;
	}
}

void Wait_Sem(int semid, int semnum){
	struct sembuf s;
	s.sem_op = -1;
	s.sem_num = semnum;
	s.sem_flg = 0;
	int r = semop(semid, &s, 1);
	checkError(r, "Errore wait sem\n");
}

void Signal_Sem(int semid, int semnum){
	struct sembuf s;
	s.sem_op = 1;
	s.sem_num = semnum;
	s.sem_flg = 0;
	int r = semop(semid, &s, 1);
	checkError(r, "Errore signal sem\n");
}
