#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"

int main(){
	int qid = open_queue(ftok(FTOK_PATH_Q, FTOK_CHAR_Q));
	if(qid==-1) _exit(1);
	
	msg_calc msg;
	int n[2] = {0, 0}; // conteggio dei messaggi di p1 e p2	
	float m[2] = {0, 0}; // media dei valori dei messaggi di p1 e p2

	while(n[0] + n[1] < P1_N_MSG + P2_N_MSG){
		int result = msgrcv(qid, (void*) &msg, sizeof(msg_calc)-sizeof(long), 0, 0);
		if(result < 0){
			perror("Errore ricezione messaggio \n");
		}
		int index = msg.processo - 1;
		n[index] += 1;
		int N = n[index];
		float M = m[index];
		m[index] = ( M * (N - 1) + msg.numero ) / N;
		printf("Media valori dei messaggi di P%d con conteggio %d : %f\n", msg.processo, N, m[index]);
	}
return 0;
}
