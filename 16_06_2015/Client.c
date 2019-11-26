#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#include "header.h"

#define N_MSG 4

int main(){
	key_t keyval = ftok(KEY_PATH, KEY_CHAR);
	int msqid;
	if((msqid = getQueue(keyval)) == -1){
		_exit(1);
	}
	srand(time(NULL)*getpid());
	data D;	
	for(int i=0; i<N_MSG; i++){
		D.pid = getpid();
		D.val[0] = rand() % 11;
		D.val[1] = rand() % 11;
		D.sum = 0;
		printf("Processo %d, messaggio numero %d, valori generati: %d %d\n", getpid(), i+1, D.val[0], D.val[1]);
		sendSincrona(msqid, &D);
	}
	_exit(0);
}
