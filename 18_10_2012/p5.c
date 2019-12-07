#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

int main(){
	printf("Processo p5: pid %d\n", getpid());
	int qid[2];
	for(int i=1; i<3; i++){
		qid[i-1] = msgget(ftok(KEY_PATH, KEY_CHAR + 3 + (i-1)*2), 0);
		checkError(qid[i-1], "Errore ottenimento coda\n");
	}
	int r;
	msgCalc msg;
	int result = 0;

	for(int t=0; t<3; t++){
		r = msgrcv(qid[1], &msg, sizeof(msgCalc)-sizeof(long), OP, 0);
		checkError(r, "Errore ricezione messaggio\n");
		for(int i=0; i < msg.numOperandi; i++){
			result += msg.operandi[i];
		}
		msg.operandi[0] = result;
		msg.numOperandi = 1;
		msg.tipo = R0 + 4;
		r = msgsnd(qid[0], &msg, sizeof(msgCalc)-sizeof(long), 0);
		checkError(r, "Errore invio messaggio\n");
		result = 0;
	}
	_exit(0);
}
