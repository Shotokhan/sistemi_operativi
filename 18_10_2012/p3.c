#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

int main(){
	printf("Processo p3: pid %d\n", getpid());
	int qid[4];
	int q[4] = {1, 3, 5, 6};
	for(int i=0; i<4; i++){
		qid[i] = msgget(ftok(KEY_PATH, KEY_CHAR + q[i]), 0);
		checkError(qid[i], "Errore ottenimento coda\n");
	}
	int r;
	msgCalc msg;
	int result = 1;
	int op1[2];
	int op2[2];

	for(int t=0; t<3; t++){
		r = msgrcv(qid[1], &msg, sizeof(msgCalc)-sizeof(long), OP, 0);
		checkError(r, "Errore ricezione messaggio\n");
		for(int i=0; i < msg.numOperandi; i++){
			if(i < 2){
				op1[i] = msg.operandi[i];
			} else{
				op2[i-2] = msg.operandi[i];
			}
		}
		opSend(&msg, qid[2], 2, op1);
		opSend(&msg, qid[3], 2, op2);
		
		for(int i=0; i<2; i++){
			r = msgrcv(qid[1], &msg, sizeof(msgCalc)-sizeof(long), R0 + 4 + i, 0);
			checkError(r, "Errore ricezione messaggio\n");
			result *= msg.operandi[0];
		}

		msg.operandi[0] = result;
		msg.numOperandi = 1;
		msg.tipo = R0 + 2;
		r = msgsnd(qid[0], &msg, sizeof(msgCalc)-sizeof(long), 0);
		checkError(r, "Errore invio messaggio\n");
		result = 1;
	}
	_exit(0);
}
