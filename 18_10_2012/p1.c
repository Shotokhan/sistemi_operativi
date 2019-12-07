#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

#include "header.h"

int main(){
	printf("Processo p1: pid %d\n", getpid());
	int qid[4];
	for(int i=1; i < 5; i++){
		qid[i-1] = msgget(ftok(KEY_PATH, KEY_CHAR + i), 0);
		checkError(qid[i-1], "Errore ottenimento coda\n");
	}

	srand(time(NULL)*getpid());
	
	msgCalc msg;
	msg.tipo = OP;
	
	char ops[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	int count=0;
	int r;
	int R[3];
	int sum=0;

	for(int t=0; t<3; t++){
		for(int i=1; i < 4; i++){
			if(i == 2){
				msgGen(&msg, qid[i], 4, ops, &count);
			} else{
				msgGen(&msg, qid[i], 2, ops, &count);
			}
		}
		


		for(int i=1; i < 4; i++){
			r = msgrcv(qid[0], &msg, sizeof(msgCalc)-sizeof(long), R0 + i, 0);
			checkError(r, "Errore ricezione messaggio\n");
			R[i-1] = msg.operandi[0];
			printf("Risultato ricevuto: R%d = %d\n", i, R[i-1]);
		}
		

		for(int i=0; i<3; i++){
			sum += R[i];
		}
		
		printf("y = %d\n", sum);
		sum = 0;
		count = 0;
	}
	_exit(0);
		
}
