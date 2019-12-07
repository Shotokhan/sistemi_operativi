#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

int main(){
	int N = 6;
	int qid[N];
	initQueues(qid, N);

	int pid;

	char* args[6][2] = {{"./p1", NULL}, {"./p2", NULL}, {"./p3", NULL}, {"./p4", NULL}, {"./p5", NULL}, {"./p6", NULL}};

	for(int i=0; i<N; i++){
		pid = fork();
		if(pid == 0){
			execvp(args[i][0], args[i]);
			perror("Errore exec\n");
		} else if(pid < 0){
			checkError(pid, "Errore creazione processo\n");
		}
	}

	waitProcesses();

	removeQueues(qid, N);
}
