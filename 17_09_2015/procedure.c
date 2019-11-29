#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <math.h>

#include "procedure.h"

void produci_elemento(int sem_id, BufferCircolare* buf){
	waitSem(sem_id, PROD, 1);
	waitSem(sem_id, MUTEX, 1);
	buf->elementi[buf->testa] = rand() % 10 + 1;
	printf("Processo %d: ho scritto %d\n", getpid(), buf->elementi[buf->testa]);
	buf->testa = (buf->testa + 1) % N;
	signalSem(sem_id, CONS, 1);
	signalSem(sem_id, MUTEX, 1);
}

void consuma_elementi(int sem_id, BufferCircolare* buf){
	waitSem(sem_id, CONS, N);
	waitSem(sem_id, MUTEX, 1);
	int valori[N];
	float somma = 0;
	float sommaQuadrati = 0;
	int i = 0;
	while(i < N){
		valori[i] = buf->elementi[buf->coda];
		buf->coda = (buf->coda + 1) % N;
		somma += valori[i];
		sommaQuadrati += valori[i] * valori[i];
		i += 1;
	}
	float media = somma / N;
	float varianza = sqrt((sommaQuadrati - media*media) / (N - 1));
	printf("Consumatore %d: media = %f, varianza = %f\n", getpid(), media, varianza);
	signalSem(sem_id, PROD, N);
	signalSem(sem_id, MUTEX);
}

void waitSem(int semid, int semnum, int n){
	struct sembuf s;
	s.sem_op = -1 * n;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(semid, &s, 1);
	if(r < 0){
		perror("Errore wait semaphore\n");
	}
}

void signalSem(int semid, int semnum, int n){
	struct sembuf s;
	s.sem_op = n;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(semid, &s, 1);
	if(r < 0){
		perror("Errore signal semaphore\n");
	}
}

void initBuffer(BufferCircolare* buf){
	for(int i=0; i<N; i++){
		buf->elementi[i] = 0;
	}
	buf->testa = 0;
	buf->coda = 0;
}
