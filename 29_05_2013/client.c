#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

#include "mex.h"

#define N_REQ 5

void checkError(int ret, char* msg);

int main(){
	srand(time(NULL)*getpid());
	int qid_req = msgget(ftok(KEY_PATH, KEY_CHAR_REQ), 0);
	checkError(qid_req, "Client - errore ottenimento coda\n");
	int qid_ans = msgget(ftok(KEY_PATH, KEY_CHAR_ANS), 0);
	checkError(qid_ans, "Client - errore ottenimento coda\n");
	client_msg c_msg;
	c_msg.tipo = getpid();
	c_msg.pid = c_msg.tipo;
	server_msg s_msg;
	for(int i=0; i<N_REQ; i++){
		c_msg.val[0] = rand() % 11;
		c_msg.val[1] = rand() % 11;
		msgsnd(qid_req, &c_msg, sizeof(client_msg)-sizeof(long), 0);
		printf("Client %d: ho inviato {%d, %d}\n", c_msg.pid, c_msg.val[0], c_msg.val[1]);
		msgrcv(qid_ans, &s_msg, sizeof(server_msg)-sizeof(long), c_msg.pid, 0);
		printf("Client %d: ho ricevuto %d\n", c_msg.pid, s_msg.result);
	}
	_exit(0);
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		_exit(1);
	}
}
