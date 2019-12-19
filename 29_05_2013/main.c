#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "mex.h"

#define N_CLIENT 3

void checkError(int ret, char* msg);

void printExitStatus(int pid, int st, char* type);

int main(){
	int pid, st;
	int qid_req = msgget(ftok(KEY_PATH, KEY_CHAR_REQ), IPC_CREAT|IPC_EXCL|0664);
	checkError(qid_req, "Main - errore creazione coda\n");
	int qid_ans = msgget(ftok(KEY_PATH, KEY_CHAR_ANS), IPC_CREAT|IPC_EXCL|0664);
	checkError(qid_ans, "Main - errore creazione coda\n");
	
	char* args_server[] = {"./server", NULL};
	char* args_client[] = {"./client", NULL};	

	pid = fork();
	if(pid == 0){
		execvp(args_server[0], args_server);
		perror("Errore exec\n");
	} else{
		checkError(pid, "Errore creazione processo\n");
	}
	
	for(int i=0; i<N_CLIENT; i++){
		pid = fork();
		if(pid == 0){
			execvp(args_client[0], args_client);
			perror("Errore exec\n");
		} else{
			checkError(pid, "Errore creazione processo\n");
		}
	}

	for(int i=0; i<N_CLIENT; i++){
		pid = wait(&st);
		printExitStatus(pid, st, "client");
	}

	client_msg msg;
	msg.tipo = 1;
	msg.pid = getpid();
	msg.val[0] = -1;
	msg.val[1] = -1;
	msgsnd(qid_req, &msg, sizeof(client_msg)-sizeof(long), 0);
	
	pid = wait(&st);
	printExitStatus(pid, st, "server");
	
	int r;
	r = msgctl(qid_req, IPC_RMID, NULL);
	checkError(r, "Errore distruzione coda richieste\n");
	r = msgctl(qid_ans, IPC_RMID, NULL);
	checkError(r, "Errore distruzione coda risposte\n");

	return 0;
}

void checkError(int ret, char* msg){
	if(ret < 0){
		perror(msg);
		_exit(0);
	}
}

void printExitStatus(int pid, int st, char* type){
	printf("Il %s %d ha terminato ", type, pid);
	if(WIFEXITED(st)){
		printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
	} else if(WIFSIGNALED(st)){
		printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
	}
}
