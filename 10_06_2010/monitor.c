#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "header.h"

void init_monitor(monitor* M, int n_cond){
	M->mutex = semget(IPC_PRIVATE, 1, IPC_CREAT|0664);
	if(M->mutex < 0){
		perror("Creazione mutex fallita\n"); _exit(1);
	}
	semctl(M->mutex, 0, SETVAL, 1);
	M->num_var_cond = n_cond;
	M->id_cond = semget(IPC_PRIVATE, M->num_var_cond, IPC_CREAT|0664);
	if(M->id_cond < 0){
		perror("Creazione gruppo semafori cond fallita\n"); _exit(1);
	}
	for(int i=0; i < M->num_var_cond; i++){
		semctl(M->id_cond, i, SETVAL, 0);
	}
	M->id_shared = shmget(IPC_PRIVATE, sizeof(int) * M->num_var_cond, IPC_CREAT|0664);
	if(M->id_shared < 0){
		perror("Creazione shared memory monitor fallita\n"); _exit(1);
	}
	M->cond = (int*) shmat(M->id_shared, 0, 0);
	for(int i=0; i < M->num_var_cond; i++){
		M->cond[i] = 0;
	}
}

void enter_monitor(monitor* M){
	Wait_Sem(M->mutex, 0);
}

void leave_monitor(monitor* M){
	Signal_Sem(M->mutex, 0);
}

void remove_monitor(monitor* M){
	semctl(M->mutex, 0, IPC_RMID);
	semctl(M->id_cond, M->num_var_cond, IPC_RMID);
	shmctl(M->id_shared, IPC_RMID, 0);
}

void wait_condition(monitor* M, int _cond){
	M->cond[_cond] += 1;
	Signal_Sem(M->mutex, 0);
	Wait_Sem(M->id_cond, _cond);
	M->cond[_cond] -= 1;
}

void signal_condition(monitor* M, int _cond){
	if(M->cond[_cond] > 0){
		Signal_Sem(M->id_cond, _cond);
	} else{
		Signal_Sem(M->mutex, 0);
	}
}

void Wait_Sem(int id_sem, int semnum){
	struct sembuf s;
	s.sem_flg = 0;
	s.sem_op = -1;
	s.sem_num = semnum;
	int r = semop(id_sem, &s, 1);
	if(r<0){perror("Wait fallita\n"); _exit(1);}
}

void Signal_Sem(int id_sem, int semnum){
	struct sembuf s;
	s.sem_flg = 0;
	s.sem_op = 1;
	s.sem_num = semnum;
	int r = semop(id_sem, &s, 1);
	if(r<0){perror("Signal fallita\n"); _exit(1);}
}
