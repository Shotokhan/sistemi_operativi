#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "header.h"

void inizioLettura(int, Buffer*);
void fineLettura(int, Buffer*);

void inizioScrittura(int, Buffer*);
void fineScrittura(int, Buffer*);

void Wait_Sem(int semid, int semnum){
	struct sembuf s;
	s.sem_flg=0;
	s.sem_num=semnum;
	s.sem_op=-1;
	int r = semop(semid, &s, 1);
	if(r<0){perror("Errore wait\n");}
}
void Signal_Sem(int semid, int semnum){
        struct sembuf s;
        s.sem_flg = 0;
        s.sem_num = semnum;
        s.sem_op = 1;
        int r = semop(semid, &s, 1);
        if(r<0) {perror("Errore signal\n");}
}

void Lettore(int ds_sem, Buffer* shm_p){
	inizioLettura(ds_sem, shm_p);
	printf("Messaggio: %d\n", shm_p->messaggio);
	fineLettura(ds_sem, shm_p);
}

void Scrittore(int ds_sem, Buffer* shm_p){
	inizioScrittura(ds_sem, shm_p);
	shm_p->messaggio = getpid();
	fineScrittura(ds_sem, shm_p);
}

void inizioLettura(int ds_sem, Buffer* shm_p){
	Wait_Sem(ds_sem, MUTEXL);
	shm_p->numLettori++;

	if(shm_p->numLettori == 1){
		Wait_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEXL);
}

void fineLettura(int ds_sem, Buffer* shm_p){
	Wait_Sem(ds_sem, MUTEXL);
	shm_p->numLettori--;
	if(shm_p->numLettori == 0){
		Signal_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEXL);
}

void inizioScrittura(int ds_sem, Buffer* shm_p){
	Wait_Sem(ds_sem, MUTEXS);
	shm_p->numScrittori++;
	
	if(shm_p->numScrittori == 1){
		Wait_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEXS);
	Wait_Sem(ds_sem, MUTEX);
}

void fineScrittura(int ds_sem, Buffer* shm_p){
	Signal_Sem(ds_sem, MUTEX);
	Wait_Sem(ds_sem, MUTEXS);
	shm_p->numScrittori--;
	
	if(shm_p->numScrittori == 0){
		Signal_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEXS);
	Signal_Sem(ds_sem, SYNCH);
}
