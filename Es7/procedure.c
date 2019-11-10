#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

void doc_aggiorna(int ds_sem, buffer* buf, char* foo){
	Wait_Sem(ds_sem, SYNCH);
	strcpy(buf->prox_appello, foo);
	printf("Nuovo appello: %s\n", buf->prox_appello);
	Signal_Sem(ds_sem, SYNCH);
}

void doc_leggi(int ds_sem, buffer* buf, int setZero){
	Wait_Sem(ds_sem, MUTEX_P);
	printf("Lettura docente %d: %d prenotati\n", getpid(), buf->num_prenotati);
	if(setZero != 0){
		buf->num_prenotati = 0;
	}
	Signal_Sem(ds_sem, MUTEX_P);
}


void stud_prenota(int ds_sem, buffer* buf){
	Wait_Sem(ds_sem, MUTEX_P);
	buf->num_prenotati += 1;
	printf("Lo studente %d si e' prenotato\n", getpid());
	Signal_Sem(ds_sem, MUTEX_P);
}

void stud_leggi(int ds_sem, buffer* buf){
	inizioLettura(ds_sem, buf);
	printf("Lettura studente %d: %s\n", getpid(), buf->prox_appello);
	fineLettura(ds_sem, buf);
}

void inizioLettura(int ds_sem, buffer* buf){
	Wait_Sem(ds_sem, MUTEX_L);
	buf->num_lettori += 1;
	if(buf->num_lettori == 1){
		Wait_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEX_L);
}

void fineLettura(int ds_sem, buffer* buf){
	Wait_Sem(ds_sem, MUTEX_L);
	buf->num_lettori -= 1;
	if(buf->num_lettori == 0){
		Signal_Sem(ds_sem, SYNCH);
	}
	Signal_Sem(ds_sem, MUTEX_L);
}

void Wait_Sem(int ds_sem, int semnum){
	struct sembuf s;
	s.sem_flg = 0;
	s.sem_op = -1;
	s.sem_num = semnum;
	int r = semop(ds_sem, &s, 1);
	if(r<0){perror("Errore wait\n");}
}

void Signal_Sem(int ds_sem, int semnum){
        struct sembuf s;
        s.sem_flg = 0;
        s.sem_num = semnum;
        s.sem_op = 1;
        int r = semop(ds_sem, &s, 1);
        if(r<0) {perror("Errore signal\n");}
}
