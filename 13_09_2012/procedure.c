#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#include "header.h"
#include "procedure.h"

void inizializza_prod_cons(PriorityProdCons *p){
	for(int i=0; i<DIM; i++){
		p->buffer[i] = 0;
	}
	p->testa = 0;
	p->coda = 0;
	init_monitor(&(p->m), NUM_VAR_COND);
}

void produci_alta_prio(PriorityProdCons *p){
	enter_monitor(&(p->m));
	printf("Prod ad alta priorita' %d, in attesa del buffer\n", getpid());
	// condizione buffer pieno
	while((p->testa + 1) % DIM == p->coda){
		wait_cond(&(p->m), H_PROD);
	}
	printf("Prod ad alta priorita' %d, attesa del buffer terminata\n", getpid());
	p->buffer[p->testa] = rand() % 13;
//	printf("Prod ad alta priorita' %d, ho scritto %d\n", getpid(), p->buffer[p->testa]);
	p->testa = (p->testa + 1) % DIM;
	signal_cond(&(p->m), CONS);
	leave_monitor(&(p->m));
}

void produci_bassa_prio(PriorityProdCons *p){
	enter_monitor(&(p->m));
	printf("Prod a bassa priorita' %d, in attesa del buffer\n", getpid());
	// condizione buffer pieno
	while((p->testa + 1) % DIM == p->coda || (p->m).cond[H_PROD] > 0){
		wait_cond(&(p->m), L_PROD);
	}
	printf("Prod a bassa priorita' %d, attesa del buffer terminata\n", getpid());
	p->buffer[p->testa] = rand() % 13 + 13;
//	printf("Prod a bassa priorita' %d, ho scritto %d\n", getpid(), p->buffer[p->testa]);
	p->testa = (p->testa + 1) % DIM;
	signal_cond(&(p->m), CONS);
	leave_monitor(&(p->m));
}

void consuma(PriorityProdCons *p){
	enter_monitor(&(p->m));
	// condizione buffer vuoto
	while(p->testa == p->coda){
		wait_cond(&(p->m), CONS);
	}
	printf("Cons %d, leggo %d\n", getpid(), p->buffer[p->coda]);
	p->coda = (p->coda + 1) % DIM;
	if((p->m).cond[H_PROD] > 0){
		signal_cond(&(p->m), H_PROD);
	} else{
		signal_cond(&(p->m), L_PROD);
	}
	leave_monitor(&(p->m));
}

void rimuovi_prod_cons(PriorityProdCons *p){
	remove_monitor(&(p->m));
}
