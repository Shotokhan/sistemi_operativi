#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#include "header.h"
#include "proc.h"

void Schedulatore(codaCircolare* q){
	int p = 0;
	// il VALORE della richiesta è il pid del processo
	pid_t disco[MEM];
	richiesta r;
	for(int i=0; i < N_REQ * N_UTENTI; i++){
		dequeue(q, &r);
		int newP = r.posizione;
		int t = ( newP > p ) ? ( newP - p ) : ( p - newP );
		printf("Schedulatore %d, la scrittura richiede %d secondi\n", getpid(), t);
		sleep(t);
		disco[newP] = r.processo;
		printf("Schedulatore %d, ho scritto %d in posizione %d\n", getpid(), r.processo, newP);
		p = newP;
	}
}

void Utente(codaCircolare* q){
	srand(time(NULL)*getpid());
	richiesta r;
	r.processo = getpid();
	for(int i=0; i<N_REQ; i++){
		r.posizione = rand() % MEM;
		enqueue(q, r);
		printf("Utente %d, ho inviato la richiesta per la posizione %d\n", r.processo, r.posizione);
	}
}

void initCoda(codaCircolare* q){
	q->id_monitor = shmget(ftok(KEY_PATH, KEY_CHAR_MONITOR), sizeof(monitorHoare), IPC_CREAT|IPC_EXCL|0664);
	checkError(q->id_monitor, "Errore creazione shared memory nella coda\n");
	q->m = (monitorHoare*) shmat(q->id_monitor, NULL, 0);
	if(q->m == (void*) -1){
		checkError(-1, "Errore shmat nella coda\n");
	}
	initMonitor(q->m, 2);
	q->testa = 0;
	q->coda = 0;
	q->numOccupati = 0;
}

void removeCoda(codaCircolare* q){
	removeMonitor(q->m);
	shmctl(q->id_monitor, IPC_RMID, NULL);
	q->m = NULL;
}

void enqueue(codaCircolare* q, richiesta r){
	enterMonitor(q->m);
	if(q->numOccupati == DIM_CODA){
		waitCondition(q->m, UTENTE);
	}
	q->req[q->coda].posizione = r.posizione;
	q->req[q->coda].processo = r.processo;
	q->coda = (q->coda + 1) % DIM_CODA;
	q->numOccupati += 1;
	signalCondition(q->m, SCHEDULER);
	leaveMonitor(q->m);
}

void dequeue(codaCircolare* q, richiesta* r){
	enterMonitor(q->m);
	if(q->numOccupati == 0){
		waitCondition(q->m, SCHEDULER);
	}
	r->posizione = q->req[q->testa].posizione;
	r->processo = q->req[q->testa].processo;
	q->testa = (q->testa + 1) % DIM_CODA;
	q->numOccupati -= 1;
	signalCondition(q->m, UTENTE);
	leaveMonitor(q->m);
}

void waitProcesses(){
	int pid, st;
	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}
}
