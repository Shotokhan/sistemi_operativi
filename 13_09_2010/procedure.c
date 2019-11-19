#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "header.h"
#include "procedure.h"

void Fornitore(monitor* M, magazzino* shm){
	int i = inizioProduzione(M, shm);
	(shm->buffer[i]).id_fornitore = getpid();
	printf("Il produttore %d ha scritto\n", getpid());
	sleep(2);
	fineProduzione(M, shm, i);
}

void Cliente(monitor* M, magazzino* shm){
	int i = inizioConsumo(M, shm);
	printf("Il consumatore %d ha consumato: %d\n", getpid(), (shm->buffer[i]).id_fornitore);
	(shm->buffer[i]).id_fornitore = 0;
	sleep(2);
	fineConsumo(M, shm, i);
}

int inizioProduzione(monitor* M, magazzino* shm){
//	printf("<%d> Inizio produzione - chiedo il mutex\n", getpid());
	enter_monitor(M);
	while(shm->livello_scorte == MAX_PRODOTTI){
		wait_cond(M, PROD);
	}
	int i = 0;
//	printf("<%d> Inizio produzione - ho il mutex\n", getpid());
	while((shm->buffer[i]).stato != LIBERO && i < MAX_PRODOTTI){
		i++;
	}
	if(i == MAX_PRODOTTI){
		printf("<%d> Produttore: errore di sincronizzazione\n", getpid());
		leave_monitor(M);
		_exit(1);
	}
	(shm->buffer[i]).stato = IN_USO;
	leave_monitor(M);
//	printf("<%d> Inizio produzione - rilascio il mutex\n", getpid());
	return i;
}

void fineProduzione(monitor* M, magazzino* shm, int i){
//	printf("<%d> Fine produzione - chiedo il mutex\n", getpid());
	enter_monitor(M);
//	printf("<%d> Fine produzione - ho il mutex\n", getpid());
	(shm->buffer[i]).stato = OCCUPATO;
	shm->livello_scorte += 1;
	signal_cond(M, CONS);
	leave_monitor(M);	
//	printf("<%d> Fine produzione - rilascio il mutex\n", getpid());
}

int inizioConsumo(monitor* M, magazzino* shm){
//	printf("<%d> Inizio consumo - chiedo il mutex\n", getpid());
	enter_monitor(M);
	while(shm->livello_scorte == 0){
		wait_cond(M, CONS);
	}
	int i = 0;
//	printf("<%d> Inizio consumo - ho il mutex\n", getpid());
	while((shm->buffer[i]).stato != OCCUPATO && i < MAX_PRODOTTI){
		i++;
	}
	if(i == MAX_PRODOTTI){
		printf("<%d> Consumatore: errore di sincronizzazione\n", getpid());
		leave_monitor(M);
		_exit(1);
	}
	(shm->buffer[i]).stato = IN_USO;
	shm->livello_scorte -= 1;
	leave_monitor(M);
//	printf("<%d> Inizio consumo - rilascio il mutex\n", getpid());
	return i;
}

void fineConsumo(monitor* M, magazzino* shm, int i){
//	printf("<%d> Fine consumo - chiedo il mutex\n", getpid());
	enter_monitor(M);
//	printf("<%d> Fine consumo - ho il mutex\n", getpid());
	(shm->buffer[i]).stato = LIBERO;
	signal_cond(M, PROD);
	leave_monitor(M);
//	printf("<%d> Fine consumo - rilascio il mutex\n", getpid());
}

void initMagazzino(magazzino* shm){
	shm->livello_scorte = 0;
	for(int i = 0; i < MAX_PRODOTTI; i++){
		(shm->buffer[i]).stato = LIBERO;
		(shm->buffer[i]).id_fornitore = 0;		
	}
}
