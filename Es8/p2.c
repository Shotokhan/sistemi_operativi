#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "header.h"

int main(){
	int qid = open_queue(ftok(FTOK_PATH_Q, FTOK_CHAR_Q));
	if(qid==-1) _exit(1);
	
	msg_calc msg;
	msg.processo = P2;
	srand(time(NULL)*getpid());
	for(int i = 0; i < P2_N_MSG; i++){
		msg.numero = ( (float) (rand() % 10000) ) / 100;
		printf("Valore del messaggio generato da %d: %f\n", msg.processo, msg.numero);
		int result = msgsnd(qid, (void*) &msg, sizeof(msg_calc) - sizeof(long), IPC_NOWAIT); 
		if(result < 0){
			perror("Errore invio messaggio\n");
		}
	}
return 0;
}
