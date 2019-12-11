#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "header.h"

void mittente(int q1, int q2){
	int counter = 10;
	int pid;
	msg messaggio;
	
	pid = fork();
	if(pid == 0){
		int r = msgrcv(q2, &messaggio, sizeof(messaggio)-sizeof(long), REMOVE_Q, 0);
		checkError(r, "Errore ricezione messaggio\n");
		printf("Daemon - Uccisione mittente e rimozione code\n");
		r = kill(getppid(), 9);
		checkError(r, "Errore kill\n");
		removeQueues(q1, q2);
		_exit(0);
		
	} else checkError(pid, "Errore creazione daemon\n");

	while(1){
		inviaMsg(q1, q2, counter, pid);
		counter += 1;
		if(counter > 100000){
			counter = 10;
		}
	}
}

void ricevente(int q2){
	int cntMsg = 0;
	msg messaggio;
	messaggio.end = 0;
	msgCnt mex;
	int r, cnt;
	char buffer[MAX_DIM];
	int len = 0;
	buffer[0] = '\0';
	while(1){
		r = msgrcv(q2, &mex, sizeof(mex)-sizeof(long), CNT_TYPE, 0);
		checkError(r, "Errore ricezione messaggio\n");
		cnt = mex.cnt;
		printf("\nMessaggio ricevuto: \n");
		while(messaggio.end == 0){
			r = msgrcv(q2, &messaggio, sizeof(messaggio)-sizeof(long), cnt, 0);
			checkError(r, "Errore ricezione messaggio\n");
			if((strlen(messaggio.data) + len) >= MAX_DIM){
				printf("%s", buffer);
				strcpy(buffer, "");
			}
			strcat(buffer, messaggio.data);
			len = strlen(buffer);
		}
		printf("%s\n", buffer);
		strcpy(buffer, "");
		len = 0;
		messaggio.end = 0;
		if(cnt == EXIT_TYPE){
			printf("Ricevente - Terminazione chat\n");
			messaggio.tipo = REMOVE_Q;
			r = msgsnd(q2, &messaggio, sizeof(messaggio)-sizeof(long), 0);
			checkError(r, "Errore invio messaggio\n");
			_exit(0);
		}
		printf("\nInserire messaggio:\n");
	}
}

void inviaMsg(int q1, int q2, int counter, int daemon){
	msg messaggio;
	for(int i=0; i<11; i++){
		messaggio.data[i] = '\0';
	}
	messaggio.end = 0;
	msgCnt mex;
	mex.tipo = CNT_TYPE;
	int r;
	int cntMsg = 0;
	messaggio.tipo = counter;
	printf("\nInserire messaggio:\n");
	scanf("%10s", messaggio.data);
	if((strcmp(messaggio.data, EXIT_MSG))==0){
		messaggio.tipo = EXIT_TYPE;
		mex.cnt = EXIT_TYPE;
		messaggio.end = 1;
		r = msgsnd(q2, &mex, sizeof(mex)-sizeof(long), 0);
		checkError(r, "Errore invio messaggio\n");
		r = msgsnd(q2, &messaggio, sizeof(messaggio)-sizeof(long), 0);
		checkError(r, "Errore invio messaggio\n");
	} else{
		mex.cnt = counter;
	}
	r = msgsnd(q1, &mex, sizeof(mex)-sizeof(long), 0);
	checkError(r, "Errore invio messaggio\n");
	r = msgsnd(q1, &messaggio, sizeof(messaggio)-sizeof(long), 0);
	checkError(r, "Errore invio messaggio\n");
	cntMsg += 1;
	char c = fgetc(stdin);
	// se è stata inserita una stringa più lunga o con spazi, invio altri messaggi
	// in stile pacchetti, tutti dello stesso tipo, in modo
	// che il ricevente possa ricostruire il messaggio originale
	while(c != '\n'){
		messaggio.data[0] = c;
		scanf("%9s", messaggio.data + sizeof(char));
		r = msgsnd(q1, &messaggio, sizeof(messaggio)-sizeof(long), 0);
		checkError(r, "Errore invio messaggio\n");
		cntMsg += 1;
		c = fgetc(stdin);
	}
	strcpy(messaggio.data, "\0\0\0\0\0\0\0\0\0\0\0");
	messaggio.end = 1;
	r = msgsnd(q1, &messaggio, sizeof(messaggio)-sizeof(long), 0);
	checkError(r, "Errore invio messaggio\n");
	if(messaggio.tipo == EXIT_TYPE){
		printf("Mittente - Terminazione chat\n");
		r = kill(daemon, 9);
		checkError(r, "Errore kill\n");
		_exit(0);
	} /* else{
		printf("Messaggio inviato, scomposto in %d messaggi\n", cntMsg);
	} */
}

void checkError(int retVal, char* msg){
	if(retVal < 0){
		perror(msg);
		_exit(1);
	}
}

void initQueues(int* q1, int* q2, char c1, char c2){
	*q1 = msgget(ftok(KEY_PATH, c1), IPC_CREAT|0666);
	checkError(*q1, "Errore creazione coda\n");
	*q2 = msgget(ftok(KEY_PATH, c2), IPC_CREAT|0666);
	checkError(*q2, "Errore creazione coda\n");
}

void removeQueues(int q1, int q2){
	int r;
	r = msgctl(q1, IPC_RMID, 0);
	checkError(r, "Errore eliminazione coda\n");
	r = msgctl(q2, IPC_RMID, 0);
	checkError(r, "Errore eliminazione coda\n");
}
