#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <stdlib.h>

#include "header.h"

void Produttore(int ds_sem, buffer* buf_ptr, int key){
	Wait_Sem(ds_sem, SPAZIO_DSP);
	Wait_Sem(ds_sem, MUTEX_P);
	int i = 0;
	while(i < DIM && buf_ptr->stato[i] != VUOTO){
		i++;
	}
	buf_ptr->stato[i] = IN_USO;
	// in realtà i < DIM è garantito dal semaforo SPAZIO_DSP
	Signal_Sem(ds_sem, MUTEX_P);
	int delay = rand() % 3 + 1;
	sleep(delay);
	buf_ptr->chiave[i] = key;
	buf_ptr->valore[i] = rand() % 100;
	buf_ptr->stato[i] = PIENO;
	printf("Produzione: <K,V> = <%d,%d>\n", buf_ptr->chiave[i], buf_ptr->valore[i]);
	Signal_Sem(ds_sem, MSG_DSP);
}

void Consumatore(int ds_sem, buffer* buf_ptr, int key){
	Wait_Sem(ds_sem, MSG_DSP);
	int key_found = 0;
	int i;
	do{
		Wait_Sem(ds_sem, MUTEX_C);
		i = 0;
		while(i < DIM && buf_ptr->chiave[i] != key){
			i++;
		}
		// in questo caso i < DIM è una condizione necessaria
		// è inoltre necessario controllare che la chiave non sia stata già consumata
		if(i < DIM){
			if(buf_ptr->stato[i] == PIENO){
				key_found = 1;
				buf_ptr->stato[i] = IN_USO;
			}
		}
		Signal_Sem(ds_sem, MUTEX_C);
	} while(key_found == 0);
	int delay = rand() % 3 + 1;
	sleep(delay);
	printf("Consumo: <K,V> = <%d,%d>\n", buf_ptr->chiave[i], buf_ptr->valore[i]);
	buf_ptr->stato[i] = VUOTO;
	Signal_Sem(ds_sem, SPAZIO_DSP);
}

void Wait_Sem(int ds_sem, int semnum){
	struct sembuf s;
	s.sem_op = -1;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(ds_sem, &s, 1);
	if(r<0){perror("Errore wait\n");}
}

void Signal_Sem(int ds_sem, int semnum){
	struct sembuf s;
	s.sem_op = 1;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(ds_sem, &s, 1);
	if(r<0){perror("Errore signal\n");}
}
