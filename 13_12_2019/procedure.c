#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <time.h>

#include "header.h"

void inserisci_operando(MonitorOperandi* op, int operando){
	pthread_mutex_lock(&(op->mutex));
	while(op->numPieni == 5){
		pthread_cond_wait(&(op->prod), &(op->mutex));
	}
	op->operando[op->testa] = operando;
	op->testa = (op->testa + 1) % 5;
	op->numPieni += 1;
	pthread_cond_signal(&(op->cons));
	pthread_mutex_unlock(&(op->mutex));
}

int preleva_operando(MonitorOperandi* op){
	int operando;
	pthread_mutex_lock(&(op->mutex));
	while(op->numPieni == 0){
		pthread_cond_wait(&(op->cons), &(op->mutex));
	}
	operando = op->operando[op->coda];
	op->coda = (op->coda + 1) % 5;
	op->numPieni -= 1;
	pthread_cond_signal(&(op->prod));
	pthread_mutex_unlock(&(op->mutex));
	return operando;
}

void inserisci_risultato(MonitorRisultati* r, int risultato){
	pthread_mutex_lock(&(r->mutex));
	while(r->numPieni == 5){
		pthread_cond_wait(&(r->prod), &(r->mutex));
	}
	r->risultato[r->testa] = risultato;
	r->testa = (r->testa + 1) % 5;
	r->numPieni += 1;
	pthread_cond_signal(&(r->cons));
	pthread_mutex_unlock(&(r->mutex));
}

int preleva_risultato(MonitorRisultati* r){
	int risultato;
	pthread_mutex_lock(&(r->mutex));
	while(r->numPieni == 0){
		pthread_cond_wait(&(r->cons), &(r->mutex));
	}
	risultato = r->risultato[r->coda];
	r->coda = (r->coda + 1) % 5;
	r->numPieni -= 1;
	pthread_cond_signal(&(r->prod));
	pthread_mutex_unlock(&(r->mutex));
	return risultato;
}

void inizializza(MonitorOperandi* op, MonitorRisultati* r){
	int R;
	R = pthread_mutex_init(&(op->mutex), 0);
	checkError(R, "Errore creazione mutex\n");
	R = pthread_cond_init(&(op->prod), 0);
	checkError(R, "Errore creazione cond\n");
	R = pthread_cond_init(&(op->cons), 0);
	checkError(R, "Errore creazione cond\n");
	R = pthread_mutex_init(&(r->mutex), 0);
	checkError(R, "Errore creazione mutex\n");
	R = pthread_cond_init(&(r->prod), 0);
	checkError(R, "Errore creazione cond\n");
	R = pthread_cond_init(&(r->cons), 0);
	checkError(R, "Errore creazione cond\n");
	for(int i=0; i<5; i++){
		op->operando[i] = 0;
		r->risultato[i] = 0;
	}
	op->testa = 0;
	op->coda = 0;
	op->numPieni = 0;
	r->testa = 0;
	r->coda = 0;
	r->numPieni = 0;
}


void rimuovi(MonitorOperandi* op, MonitorRisultati* r){
	int R;
	R = pthread_mutex_destroy(&(op->mutex));
	checkError(R, "Errore distruzione mutex\n");
	R = pthread_cond_destroy(&(op->prod));
	checkError(R, "Errore distruzione cond\n");
	R = pthread_cond_destroy(&(op->cons));
	checkError(R, "Errore distruzione cond\n");
	R = pthread_mutex_destroy(&(r->mutex));
	checkError(R, "Errore distruzione mutex\n");
	R = pthread_cond_destroy(&(r->prod));
	checkError(R, "Errore distruzione cond\n");
	R = pthread_cond_destroy(&(r->cons));
	checkError(R, "Errore distruzione cond\n");
}

void checkError(int retVal, char* msg){
	if(retVal < 0){
		perror(msg);
		pthread_exit(0);
	}
}

void* Prod(void* data){
	MonitorOperandi* op = (MonitorOperandi*) data;
	int randVal;
	for(int i=0; i<12; i++){
		randVal = rand() % 10 + 1;
		inserisci_operando(op, randVal);
		printf("Sono il thread che inserisce gli operandi, ho inserito %d\n", randVal);
	}
	pthread_exit(0);
}

void* Cons_Prod(void* data){
	dataOpRis* buf = (dataOpRis*) data;
	int op, square;
	for(int i=0; i<4; i++){
		op = preleva_operando(buf->op);
		printf("Sono il thread %d che preleva gli operandi, ho prelevato %d\n", buf->thread_id, op);
		square = op * op;
		inserisci_risultato(buf->r, square);
		printf("Sono il thread %d che inserisce i risultati, ho inserito %d\n", buf->thread_id, square);
	}
	pthread_exit(0);
}

void* Cons(void* data){
	MonitorRisultati* r = (MonitorRisultati*) data;
	int cnt_gt25 = 0;
	int cnt_leq25 = 0;
	int ris;
	for(int i=0; i<12; i++){
		ris = preleva_risultato(r);
		printf("Sono il thread che preleva i risultati, ho prelevato %d\n", ris);
		if(ris <= 25){
			cnt_leq25+=1;
		} else{
			cnt_gt25+=1;
		}
	}
	printf("Sono il thread che preleva i risultati, i conteggi sono:\n Risultati <= 25: %d\n Risultati > 25: %d\n", cnt_leq25, cnt_gt25);
	pthread_exit(0);
}
