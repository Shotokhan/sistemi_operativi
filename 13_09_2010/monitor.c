#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "header.h"

void init_monitor(monitor* M, int ncond){
	M->mutex = semget(IPC_PRIVATE, 1, IPC_CREAT|IPC_EXCL|0664);
	if(M->mutex == -1){
		_exit(1);
	}
	semctl(M->mutex, 0, SETVAL, 1);
	M->num_var_cond = ncond;
	M->id_cond = semget(IPC_PRIVATE, ncond, IPC_CREAT|IPC_EXCL|0664);
	if(M->id_cond == -1){
		_exit(1);
	}
	for(int i = 0; i < ncond; i++){
		semctl(M->id_cond, i, SETVAL, 0);
	}
	M->id_shm = shmget(IPC_PRIVATE, sizeof(int)*ncond, IPC_CREAT|IPC_EXCL|0664);
	if(M->id_shm == -1){
		_exit(1);
	}
	M->cond = (int*) shmat(M->id_shm, 0, 0);
	for(int i = 0; i < ncond; i++){
		M->cond[i] = 0;
	}
}

void Wait_Sem(int sem, int semnum){
	struct sembuf s;
	s.sem_num = semnum;
	s.sem_flg = 0;
	s.sem_op = -1;
	int r = semop(sem, &s, 1);
	if(r<0){perror("Wait fallita\n");}
}	

void Signal_Sem(int sem, int semnum){
	struct sembuf s;
	s.sem_num = semnum;
	s.sem_flg = 0;
	s.sem_op = 1;
	int r = semop(sem, &s, 1);
	if(r<0){perror("Signal fallita\n");}
}

void enter_monitor(monitor* M){
	Wait_Sem(M->mutex, 0);
}

void leave_monitor(monitor* M){
	Signal_Sem(M->mutex, 0);
}

// Signal and continue
void signal_cond(monitor* M, int condInd){
	if(M->cond[condInd] > 0){
		Signal_Sem(M->id_cond, condInd);
	}
}

void wait_cond(monitor* M, int condInd){
	M->cond[condInd] += 1;
	Signal_Sem(M->mutex, 0);
	Wait_Sem(M->id_cond, condInd);
	M->cond[condInd] -= 1;
	Wait_Sem(M->mutex, 0);	
}
/*
void signal_all(monitor* M, int condInd){
	while(M->cond[condInd] > 0){
		Signal_Sem(M->id_cond, condInd);
	}
}
*/
void delete_monitor(monitor* M){
	semctl(M->mutex, 0, IPC_RMID);
	semctl(M->id_cond, M->num_var_cond, IPC_RMID);
	shmctl(M->id_shm, IPC_RMID, 0);
}
