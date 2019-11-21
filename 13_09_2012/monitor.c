#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#include "header.h"

void enter_monitor(monitor* m){
	Wait_Sem(m->mutex, 0);
}

void leave_monitor(monitor* m){
	Signal_Sem(m->mutex, 0);
}

void signal_cond(monitor* m, int id_cond){
	if(m->cond[id_cond] > 0){
		Signal_Sem(m->condsem, id_cond);
	}
}

void wait_cond(monitor* m, int id_cond){
	m->cond[id_cond] += 1;
	Signal_Sem(m->mutex, 0);
	Wait_Sem(m->condsem, id_cond);
	m->cond[id_cond] -= 1;
	Wait_Sem(m->mutex, 0);
}

void init_monitor(monitor* m, int n_cond){
	m->mutex = semget(IPC_PRIVATE, 1, IPC_CREAT|0664);
	if(m->mutex < 0){perror("Errore semget\n"); _exit(1);}
	semctl(m->mutex, 0, SETVAL, 1);
	m->n_cond = n_cond;
	m->condsem = semget(IPC_PRIVATE, n_cond, IPC_CREAT|0664);
	if(m->condsem < 0){perror("Errore semget\n"); _exit(1);}
	m->id_shm = shmget(IPC_PRIVATE, sizeof(int)*n_cond, IPC_CREAT|0664);
	if(m->id_shm < 0){perror("Errore shmget\n"); _exit(1);}
	m->cond = shmat(m->id_shm, 0, 0);
	for(int i = 0; i < n_cond; i++){
		m->cond[i] = 0;
		semctl(m->condsem, i, SETVAL, 0);
	}
}

void remove_monitor(monitor* m){
	semctl(m->mutex, 0, IPC_RMID);
	semctl(m->condsem, m->n_cond, IPC_RMID);
	shmctl(m->id_shm, IPC_RMID, 0);
}

void Wait_Sem(int id_sem, int semnum){
	struct sembuf s;
	s.sem_num = semnum;
	s.sem_op = -1;
	s.sem_flg = 0;
	int r = semop(id_sem, &s, 1);
	if(r<0){perror("Errore wait sem\n"); _exit(1);}
}

void Signal_Sem(int id_sem, int semnum){
	struct sembuf s;
	s.sem_num = semnum;
	s.sem_op = 1;
	s.sem_flg = 0;
	int r = semop(id_sem, &s, 1);
	if(r<0){perror("Errore signal sem\n"); _exit(1);}

}
