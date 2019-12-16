#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#include "header.h"

void client(int msqid){
	msg client_msg;
	client_msg.tipo = DC;
	client_msg.pid = getpid();
	printf("Sono il client %d, inizio ad inviare messaggi\n", getpid());
	for(int i=0; i<N_MSG; i++){
		msgsnd(msqid, (void*) &client_msg, sizeof(msg)-sizeof(long), 0);
		sleep(1);
	}
	printf("Sono il client %d, termino\n", getpid());
}

void server(int msqid_c, int msqid_p, int printer){
	buffer buf;
	buf.count = 0;
	printf("Sono il server %d, inizio a ricevere messaggi\n", getpid());
	for(int i=0; i < N_MSG * N_CLIENT; i++){
		msgrcv(msqid_c, (void*) &(buf.data[buf.count]), sizeof(msg)-sizeof(long), 0, 0);
		buf.count += 1;
		if(buf.count == DIM){
			printf("Sono il server %d, buffer pieno: invio al printer\n", getpid());
			while(buf.count > 0){
				msgsnd(msqid_p, (void*) &(buf.data[DIM - buf.count]), sizeof(msg)-sizeof(long), 0);
				buf.count -= 1;
			}
		}
	}
	printf("Sono il server %d, termino ed uccido il printer %d\n", getpid(), printer);
	kill(printer, 9);
}

void printer(int msqid){
	int count = 0;
	msg server_msg;
	printf("Sono il printer %d, sono online\n", getpid());
	for(int i=0; i < N_MSG * N_CLIENT; i++){
		msgrcv(msqid, (void*) &server_msg, sizeof(msg)-sizeof(long), 0, 0);
		printf("Messaggio ricevuto numero %d, proveniente dal client %d\n", count + 1, server_msg.pid);
		count += 1; // avrei potuto usare anche la stessa variabile i, ma in generale non si sa quanti messaggi devono arrivare
	}
	printf("Sono il printer %d, termino\n", getpid());
}

void init_queues(char key1, char key2, int qid[2]){
	int r = msgget(ftok(KEY_PATH, key1), IPC_CREAT|IPC_EXCL|0664);
	check_error(r, "Errore creazione coda\n");
	qid[0] = r;
	r = msgget(ftok(KEY_PATH, key2), IPC_CREAT|IPC_EXCL|0664);
	check_error(r, "Errore creazione coda\n");
	qid[1] = r;
}

void remove_queues(int msqid_c, int msqid_p){
	int r = msgctl(msqid_c, IPC_RMID, 0);
	check_error(r, "Errore distruzione coda\n");
	r = msgctl(msqid_p, IPC_RMID, 0);
	check_error(r, "Errore distruzione coda\n");
}

void check_error(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		_exit(1);
	}
}

void wait_processes(){
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
