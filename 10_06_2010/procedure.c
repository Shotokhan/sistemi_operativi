#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#include "header.h"
#include "procedure.h"

meteo* init_meteo(int* id_meteo_ptr){
	*id_meteo_ptr = shmget(IPC_PRIVATE, sizeof(meteo), IPC_CREAT|0664);
	if(*id_meteo_ptr < 0){
		perror("Creazione shared memory meteo fallita\n"); _exit(1);
	}
	meteo* ds_shm = (meteo*) shmat(*id_meteo_ptr, 0, 0);
	ds_shm->temperatura = 0;
	ds_shm->umidita = 0;
	ds_shm->pioggia = 0;
	ds_shm->numLettori = 0;
	ds_shm->numScrittori = 0;
	return ds_shm;
}

void remove_meteo(int id_meteo){
	shmctl(id_meteo, IPC_RMID, 0);
}

// il monitor deve avere 2 variabili cond

void scrittore(monitor* M, meteo* ds_shm){
	inizioScrittura(M, ds_shm);
	ds_shm->temperatura = rand() % 101 - 50;
	ds_shm->umidita = rand() % 100;
	ds_shm->pioggia = rand() % 2;
	fineScrittura(M, ds_shm);
}

void lettore(monitor* M, meteo* ds_shm){
	inizioLettura(M, ds_shm);
	printf("Lettura del proc. %d: Temp. %d, Umidita %d, Pioggia %d\n", getpid(), ds_shm->temperatura, ds_shm->umidita, ds_shm->pioggia);
	fineLettura(M, ds_shm);
}

void inizioScrittura(monitor* M, meteo* ds_shm){
	enter_monitor(M);
	if(ds_shm->numScrittori > 0 || ds_shm->numLettori > 0){
		wait_condition(M, COND_S);
	}
	ds_shm->numScrittori += 1;
	leave_monitor(M);
}

void fineScrittura(monitor* M, meteo* ds_shm){
	enter_monitor(M);
	if(M->cond[COND_S] > 0){
		signal_condition(M, COND_S);
	} else if(M->cond[COND_L] > 0){
		signal_condition(M, COND_L);
	}
	ds_shm->numScrittori -= 1;
	leave_monitor(M);
}

void inizioLettura(monitor* M, meteo* ds_shm){
	enter_monitor(M);
	if(ds_shm->numScrittori > 0){
		wait_condition(M, COND_L);
	}
	if(M->cond[COND_L] > 0){
		signal_condition(M, COND_L);
	}
	ds_shm->numLettori += 1;
	leave_monitor(M);
}

void fineLettura(monitor* M, meteo* ds_shm){
	enter_monitor(M);
	ds_shm->numLettori -= 1;
	if(ds_shm->numLettori == 0 && M->cond[COND_S] > 0){
		signal_condition(M, COND_S);
	}
	leave_monitor(M);
}
